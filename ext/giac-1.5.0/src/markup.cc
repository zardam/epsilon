/*  markup.cc
 *
* copyright (c) 2019 Luka Marohnić
 *
* This program is free software;you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation;either version 3 of the License,or
* (at your option) any later version.
 *
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY;without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
 *
* You should have received a copy of the GNU General Public License
* along with this program. If not,see <http://www.gnu.org/licenses/>.
 */

#include "giacPCH.h"
#include "giac.h"
#include "markup.h"
#include "signalprocessing.h"
#include "tex.h"
#include "mathml.h"
#ifdef __ANDROID__
using std::vector;
#endif
#if defined GIAC_HAS_STO_38 || defined NSPIRE || defined NSPIRE_NEWLIB ||      \
    defined FXCG || defined GIAC_GGB || defined USE_GMP_REPLACEMENTS
inline bool is_graphe(const giac::gen &g,std::string &disp_out,
                      const giac::context *) {
  return false;
}
#else
#include "graphtheory.h"
#endif

using namespace std;

#ifndef NO_NAMESPACE_GIAC
namespace giac {
#endif // ndef NO_NAMESPACE_GIAC

string mml_itimes="<mo>&it;</mo>";
string mml_cdot="<mo>&middot;</mo>";
string mml_dot="<mo>.</mo>";
string mml_times="<mo>&times;</mo>";
string mml_plus="<mo>+</mo>";
string mml_minus="<mo>&minus;</mo>";
string mml_apply="<mo>&af;</mo>";
string mml_icomma="<mo>&ic;</mo>";
string mml_i="<mi mathvariant='normal'>i</mi>";
string mml_e="<mi mathvariant='normal'>e</mi>";
string mml_d="<mi mathvariant='normal'>d</mi>";
string mml_re="<mi>&realpart;</mi>";
string mml_im="<mi>&imagpart;</mi>";

string tex_itimes="\\,";

bool is_texmacs_compatible_latex_export=false;
bool force_legacy_conversion_to_latex=false;

enum OperatorPrecedence {
  _PRIORITY_APPLY=1, // function application,array access
  _PRIORITY_UNARY=2, // unary operator
  _PRIORITY_EXP=3,   // power or exponential
  _PRIORITY_MUL=4,   // multiplication,division,modulo
  _PRIORITY_ADD=5,   // addition,subtraction
  _PRIORITY_SET=6,   // set operator
  _PRIORITY_INEQ=7,  // inequality
  _PRIORITY_EQ=8,    // equation
  _PRIORITY_COMP=9,  // comparison operator==or !=
  _PRIORITY_NOT=10,  // logical negation
  _PRIORITY_AND=11,  // logical conjunction
  _PRIORITY_XOR=12,  // logical exclusive or
  _PRIORITY_OR=13,   // logical disjunction
  _PRIORITY_COND=14, // conditional expression
  _PRIORITY_ASSGN=15,// assignment operator
  _PRIORITY_OTHER=100
};

enum MarkupBlockTypeFlags {
  _MLBLOCK_GENERAL=0,
  _MLBLOCK_NUMERIC_EXACT=1,
  _MLBLOCK_NUMERIC_APPROX=2,
  _MLBLOCK_SUBTYPE_IDNT=4,
  _MLBLOCK_FRACTION=8,
  _MLBLOCK_ROOT=16,
  _MLBLOCK_NEG=32,
  _MLBLOCK_SUMPROD=64,
  _MLBLOCK_FACTORIAL=128,
  _MLBLOCK_MATRIX=256,
  _MLBLOCK_ELEMAPP=512,
  _MLBLOCK_FUNC=1024,
  _MLBLOCK_POWER=2048,
  _MLBLOCK_DERIVATIVE=4096,
  _MLBLOCK_HAS_SUBSCRIPT=8192
};

enum MarkupFlags {
  _MARKUP_LATEX=1,
  _MARKUP_TOPLEVEL=2,
  _MARKUP_UNIT=4,
  _MARKUP_MATHML_PRESENTATION=8,
  _MARKUP_MATHML_CONTENT=16,
  _MARKUP_ELEMPOW=32,
  _MARKUP_FACTOR=64,
  _MARKUP_CODE=128,
  _MARKUP_QUOTE=256,
  _MARKUP_ERROR=512
};

bool is_substr(const string &s,const string &needle) {
  return s.find(needle)!=string::npos;
}

bool is_prefix(const string &s,const string &p) {
  return s.find(p)==0;
}

bool is_suffix(const string &s,const string &p) {
  return s.rfind(p)+p.length()==s.length();
}

bool is_latex_command_suffix(const string &s,const string &com,bool delim=false) {
  string c=com+"{";
  size_t pos=s.rfind(c);
  if (pos==string::npos || (delim && pos>0 && (isalnum(s[pos-1]) || s[pos-1]==32)))
    return false;
  pos+=c.length();
  int level=1;
  while (pos<s.length() && level>0) {
    if (s[pos]=='{' && s[pos-1]!='\\')
      ++level;
    else if (s[pos]=='}' && s[pos-1]!='\\')
      --level;
    ++pos;
  }
  assert(level==0);
  return pos==s.length();
}

bool is_leading_bracket(const string &s) {
  return is_prefix(s,"\\left(") || is_prefix(s,"\\left\\{") || is_prefix(s,"\\left[");
}

bool is_closing_bracket(const string &s) {
  return is_suffix(s,"\\right)") || is_suffix(s,"\\right\\}") || is_suffix(s,"\\right]");
}

bool is_greek_letter(const string &s) {
  switch (s.size()) {
  case 2:
    return s=="mu" || s=="nu" || s=="pi" || s=="Pi" || s=="xi" ||
           s=="Xi";
  case 3:
    return s=="chi" || s=="phi" || s=="Phi" || s=="eta" || s=="rho" ||
           s=="tau" || s=="psi" || s=="Psi";
  case 4:
    return s=="beta" || s=="zeta";
  case 5:
    return s=="alpha" || s=="delta" || s=="Delta" || s=="gamma" ||
           s=="Gamma" || s=="kappa" || s=="theta" || s=="Theta" ||
           s=="sigma" || s=="Sigma" || s=="Omega" || s=="omega";
  case 6:
    return s=="lambda" || s=="Lambda";
  case 7:
    return s=="epsilon";
  default:
    break;
  }
  return false;
}

bool is_tex_greek__suffix(const string &s) {
  if (!is_suffix(s,"\\_")) return false;
  int i;
  for (i=s.size()-2;i-->0;) {
    if (s[i]=='\\') break;
  }
  if (i<0) return false;
  return is_greek_letter(s.substr(i+1,s.size()-i-3));
}

bool is_double_letter(const string &s) {
  return s.length()==2 && isalpha(s.at(0)) && s.at(0)==s.at(1);
}

typedef struct {
  int priority;
  int type;
  bool neg;
  string latex;
  string markup;
  string content;
  int split_pos;
  int split_pos_tex;
  bool appl;
  bool ctype(int t) const { return (type & t)!=0; }
} MarkupBlock;

extern MarkupBlock gen2markup(const gen &g,int flags,int &idc,GIAC_CONTEXT);

string tex_implmul(const MarkupBlock &ml,const MarkupBlock &prev) {
  if (is_texmacs_compatible_latex_export &&
      (((is_latex_command_suffix(prev.latex,"^",true) || is_latex_command_suffix(prev.latex,"_",true)) &&
        !is_leading_bracket(ml.latex)) ||
       is_tex_greek__suffix(prev.latex) ||
       (is_prefix(ml.latex,"\\mathrm{") &&
        !is_substr(ml.latex.substr(8,ml.latex.find("}",8)-8),"\\_")) ||
       (is_prefix(ml.latex,"\\operatorname{") &&
        (prev.latex.empty() || !isdigit(prev.latex[prev.latex.size()-1])) &&
        !is_latex_command_suffix(prev.latex,"^") && !is_latex_command_suffix(prev.latex,"_")) ||
       is_latex_command_suffix(prev.latex,"\\mathrm") ||
       is_latex_command_suffix(prev.latex,"\\operatorname")))
    return "\\,";
  else if (!is_texmacs_compatible_latex_export &&
           (ml.ctype(_MLBLOCK_ELEMAPP) || is_prefix(ml.latex,"\\operatorname{") ||
            is_leading_bracket(ml.latex) || is_closing_bracket(prev.latex)))
    return " ";
  return tex_itimes;
}

#ifndef KHICAS
void enable_texmacs_compatible_latex_export(bool yes) {
  is_texmacs_compatible_latex_export=yes;
  tex_itimes=(yes?" ":"\\,");  
}
#endif
  
#define NUM_MATHML_CONTENT_ELEMENTS 31
const string mathml_content_elements[NUM_MATHML_CONTENT_ELEMENTS]={
  "cn",
  "ci",
  "cs",
  "csymbol",
  "cerror",
  "share",
  "bind",
  "bvar",
  "semantics",
  "apply",
  "domainofapplication",
  "interval",
  "condition",
  "lowlimit",
  "uplimit",
  "degree",
  "momentabout",
  "logbase",
  "inverse",
  "lambda",
  "piecewise",
  "piece",
  "otherwise",
  "set",
  "list",
  "vector",
  "matrix",
  "matrixrow",
  "declare",
  "reln",
  "fn"
};

bool is_mathml_content_element(const string &s) {
  for (int i=0;i<NUM_MATHML_CONTENT_ELEMENTS;++i) {
    if (mathml_content_elements[i]==s) return true;
  }
  return false;
}

string mml_tag(const string &tag,const string &str,int idc=0,
               const string &attr_key1="",const string &attr_val1="",
               const string &attr_key2="",const string &attr_val2="",
               const string &attr_key3="",const string &attr_val3="",
               const string &attr_key4="",const string &attr_val4="") {
  string r,attr1,attr2,attr3,attr4;
  if (tag[0]=='c' || is_mathml_content_element(tag))
    r="id";
  else if (tag[0]=='m')
    r="xref";
  assert(!r.empty());
  if (!attr_key1.empty() && !attr_val1.empty())
    attr1=" "+attr_key1+"='"+attr_val1+"'";
  if (!attr_key2.empty() && !attr_val2.empty())
    attr2=" "+attr_key2+"='"+attr_val2+"'";
  if (!attr_key3.empty() && !attr_val3.empty())
    attr3=" "+attr_key3+"='"+attr_val3+"'";
  if (!attr_key4.empty() && !attr_val4.empty())
    attr4=" "+attr_key4+"='"+attr_val4+"'";
  if (idc==0)
    return "<"+tag+attr1+attr2+attr3+attr4+">"+str+"</"+tag+">";
  return "<"+tag+" "+r+"='id"+gen(idc).print(context0)+"'"+attr1+attr2+attr3+attr4+">"+str+"</"+tag+">";
}

string mml_csymbol(const string &s,const string &cd) {
  string attr=(cd.empty()?"":"cd");
  return mml_tag("csymbol",s,0,attr,cd);
}

string extract_id(MarkupBlock &ml,bool content) {
  size_t pos_id_start,pos_gt,pos_id_end;
  string &str=(content?ml.content:ml.markup);
  string attr=(content?" id='":" xref='");
  pos_gt=str.find(">");
  pos_id_start=str.find(attr);
  if (pos_id_start==string::npos || pos_id_start>pos_gt)
    return "";
  pos_id_end=str.find("'",pos_id_start+attr.length());
  assert(pos_id_end!=string::npos);
  string ret=str.substr(pos_id_start+attr.length(),
                          pos_id_end-pos_id_start-attr.length());
  str=str.substr(0,pos_id_start)+str.substr(pos_id_end+1);
  return ret;
}

string insert_id(const string &str,int idc,bool content) {
  if (idc==0)
    return str;
  size_t pos=str.find(">");
  if (pos==string::npos || pos==0)
    return str;
  if (str[pos-1]=='/') pos--;
  string r=(content?" id='id":" xref='id");
  return str.substr(0,pos)+r+gen(idc).print(context0)+"'"+str.substr(pos);
}

string trim_string(const string &s_orig,int &indent) {
  string s=s_orig;
  size_t i,j;
  indent=0;
  for (i=0;i<s.length();++i) {
    char c=s.at(i);
    if (c==' ')
      indent++;
    else if (c!='\n' && c!='\t')
      break;
  }
  for (j=s.length();j-->0;) {
    char c=s.at(j);
    if (c!=' ' && c!='\n' && c!='\t')
      break;
  }
  return s.substr(i,j-i+1);
}

string str_to_tex(const string &s_orig,bool quote,bool ind) {
  int indent;
  string s=trim_string(s_orig,indent),ret;
  for (string::const_iterator it=s.begin();it!=s.end();++it) {
    switch (*it) {
    case '\\': case '%': case '_': case '$': case '{': case '}': case '^':
      ret+="\\";
      ret+=string(1,*it);
      break;
    default:
      ret+=string(1,*it);
      break;
    }
  }
  if (ind && indent>0)
    for (int i=0;i<indent;++i) ret="\\ "+ret;
  if (quote)
    return "\""+ret+"\"";
  return ret;
}

string str_to_mml(const string &s_orig,bool ind,const string &tag="",int idc=0) {
  int indent;
  string s=trim_string(s_orig,indent),ret;
  for (string::const_iterator it=s.begin();it!=s.end();++it) {
    switch (*it) {
    case '&':
      ret+="&amp;";
      break;
    case '<':
      ret+="&lt;";
      break;
    case '>':
      ret+="&gt;";
      break;
    default:
      ret+=string(1,*it);
      break;
    }
  }
  if (!tag.empty())
    ret=mml_tag(tag,ret,idc);
  if (ind && indent>0)
    ret=mml_tag("mphantom",mml_tag("mtext",string(indent,'x')))+ret;
  return ret;
}

void string2markup(MarkupBlock &ml,const string &s_orig,int flags,int &idc) {
  bool tex=(flags & _MARKUP_LATEX)!=0;
  bool content=(flags & _MARKUP_MATHML_CONTENT)!=0;
  bool presentation=(flags & _MARKUP_MATHML_PRESENTATION)!=0;
  bool code=(flags & _MARKUP_CODE)!=0;
  bool quote=(flags & _MARKUP_QUOTE)!=0;
  bool err=(flags & _MARKUP_ERROR) !=0;
  vector<string> lines;
  size_t pos,last_pos=0,p;
  string s;
  while ((pos=s_orig.find("\n",last_pos))!=string::npos) {
    s=s_orig.substr(last_pos,pos-last_pos);
    for (p=0;p<s.length() && s.at(p)==' ';++p);
    if (p<s.length())
      lines.push_back(s);
    last_pos=pos+1;
  }
  lines.push_back(s_orig.substr(last_pos));
  if (tex) {
    string textext=code?"\\texttt":"\\text";
    for (vector<string>::const_iterator it=lines.begin();it!=lines.end();++it) {
      ml.latex+=(it!=lines.begin()?"\\\\":"")+textext+"{"+str_to_tex(*it,quote,code)+"}";
    }
    if (lines.size()>1)
      ml.latex="\\begin{array}{l}"+ml.latex+"\\end{array}";
  }
  if (content) {
    if (err)
      ml.content=mml_tag("cerror",str_to_mml(s_orig,false,"cs"),++idc);
    else
      ml.content=str_to_mml(s_orig,false,"cs",++idc);
  }
  if (presentation) {
    string mtag=quote?"ms":"mtext";
    if (lines.size()==1)
      ml.markup=mml_tag(mtag,str_to_mml(lines.front(),false),err?0:idc,
                          code?"mathvariant":"","monospace");
    else {
      for (vector<string>::const_iterator it=lines.begin();it!=lines.end();++it) {
        ml.markup+="<mtr><mtd>"+str_to_mml(*it,code,mtag)+"</mtd></mtr>";
      }
      ml.markup=mml_tag("mtable",ml.markup,err?0:idc,
                          "columnalign","left",code?"mathvariant":"","monospace");
    }
    if (err)
      ml.markup=mml_tag("merror",ml.markup,idc);
  }
}

#define NUM_UNIT_PAIRS 48
const string unit_pairs[NUM_UNIT_PAIRS][2]={
  /* "units_imperial1" */
  { "acre","acre" },
  { "bar","bar" },
  { "degree_Fahrenheit","degreeF" },
  { "foot","ft" },
  { "mile","mile" },
  { "pint","ptUK" },
  { "pound_force","lbf" },
  { "pound_mass","lb" },
  { "yard","yd" },
  /* "units_metric1" */
  { "Coulomb","C" },
  { "Joule","J" },
  { "Newton","N" },
  { "Pascal","Pa" },
  { "Watt","W" },
  { "amp","A" },
  { "degree_Kelvin","K" },
  { "gramme","g" },
  { "litre","l" },
  { "metre","m" },
  { "second","s" },
  { "volt","V" },
  /* "units_time1" */
  { "calendar_year","yr" },
  { "day","d" },
  { "hour","h" },
  { "minute","mn" },
  /* "units_us1" */
  { "foot_us_survey","ftUS" },
  { "mile_us_survey","miUS" },
  /* "SI_BaseUnits1" */
  { "candela","cd" },
  { "mole","mol" },
  /* "SI_NamedDerivedUnits1" */
  { "becquerel","Bq" },
  { "farad","F" },
  { "gray","Gy" },
  { "henry","H" },
  { "hertz","Hz" },
  { "lumen","lm" },
  { "lux","lx" },
  { "ohm","Ohm" },
  { "radian","rad" },
  { "siemens","S" },
  { "sievert","Sv" },
  { "steradian","sr" },
  { "tesla","T" },
  { "weber","Wb" },
  /* "SIUsed_OffSystemMeasuredUnits1" */
  { "electronvolt","eV" },
  /* "SIUsed_OffSystemUnits1" */
  { "degree-of-arc","deg" },
  { "minute-of-arc","arcmin" },
  { "second-of-arc","arcs" },
  { "tonne","t" }
};

#define NUM_SI_PREFIX_PAIRS 20
const string si_prefix_pairs[NUM_SI_PREFIX_PAIRS][2]={
  { "yotta","Y" },
  { "zetta","Z" },
  { "exa","E" },
  { "peta","P" },
  { "tera","T" },
  { "giga","G" },
  { "mega","M" },
  { "kilo","kK" },
  { "hecto","hH" },
  { "deka","D" },
  { "deci","d" },
  { "centi","c" },
  { "milli","m" },
  { "micro","µ" },
  { "nano","n" },
  { "pico","p" },
  { "femto","f" },
  { "atto","a" },
  { "zepto","z" },
  { "yocto","y" }
};

string unit2content(const string &s,int idc) {
  string ret,p,cd;
  bool has_prefix=false;
  int i,j;
  for (i=0;i<NUM_UNIT_PAIRS;++i) {
    if (s==unit_pairs[i][1]) {
      ret=unit_pairs[i][0];
      break;
    }
  }
  if (ret.empty()) {
    int ofs=(s.substr(0,2)=="µ"?2:1);
    p=s.substr(0,ofs);
    for (j=0;j<NUM_SI_PREFIX_PAIRS;++j) {
      if (si_prefix_pairs[j][1].find(p)!=string::npos) {
        has_prefix=true;
        break;
      }
    }
    if (has_prefix) {
      for (i=0;i<NUM_UNIT_PAIRS;++i) {
        if (s==unit_pairs[i][1].substr(ofs)) {
          ret=unit_pairs[i][0];
          break;
        }
      }
    }
  }
  if (ret.empty())
    return mml_tag("ci",s,idc);
  if (i<9)
    cd="units_imperial1";
  else if (i<21)
    cd="units_metric1";
  else if (i<25)
    cd="units_time1";
  else if (i<27)
    cd="units_us1";
  else if (i<29)
    cd="SI_BaseUnits1";
  else if (i<43)
    cd="SI_NamedDerivedUnits1";
  else if (i<44)
    cd="SIUsed_OffSystemMeasuredUnits1";
  else
    cd="SIUsed_OffSystemUnits1";
  if (!has_prefix)
    return mml_tag("csymbol",ret,idc,"cd",cd);
  return mml_tag("apply",mml_tag("csymbol","prefix",0,"cd","units_ops1")+
                          mml_tag("csymbol",si_prefix_pairs[j][0],0,"cd","units_siprefix1")+ret,
                 idc);
}

string constant2content(const string &s,int idc) {
  string ret;
  if (s=="NA") ret="Avogadros_constant";
  else if (s=="k") ret="Boltzmann_constant";
  else if (s=="F") ret="Faradays_constant";
  else if (s=="h") ret="Planck_constant";
  else if (s=="R") ret="gas_constant";
  else if (s=="G") ret="gravitational_constant";
  else if (s=="mu0") ret="magnetic_constant";
  else if (s=="c") ret="speed_of_light";
  else return mml_tag("ci",s,idc);
  return mml_tag("csymbol",ret,idc,"cd","physical_consts1");
}

string idnt2markup(const string &s_orig,bool tex,bool unit,int idc=0) {
  if (unit && !s_orig.empty() && s_orig[0]=='_') {
    string s=s_orig.substr(1);
    if (s=="a0_")
      return tex?"a_0":mml_tag("msub","<mi>a</mi><mn>0</mn>",idc);
    else if (s=="alpha_")
      return tex?"\\alpha ":mml_tag("mi","&alpha;",idc);
    else if (s=="c_")
      return tex?"c":mml_tag("mi","c",idc);
    else if (s=="c3_")
      return tex?"b":mml_tag("mi","b",idc);
    else if (s=="epsilon0_")
      return tex?"\\varepsilon_0"
                :mml_tag("msub","<mi>&epsilon;</mi><mn>0</mn>",idc);
    else if (s=="epsilonox_")
      return tex?"\\varepsilon_{\\mathrm{SiO}_2}"
                :mml_tag("msub","<mi>&epsilon;</mi><msub><mi>SiO</mi><mn>2</mn></msub>",idc);
    else if (s=="epsilonsi_")
      return tex?"\\varepsilon_\\mathrm{Si}"
                :mml_tag("msub","<mi>&epsilon;</mi><mi>Si<mi>",idc);
    else if (s=="F_")
      return tex?"F":mml_tag("mi","F",idc);
    else if (s=="f0_")
      return tex?"f_0":mml_tag("msub","<mi>f</mi><mn>0</mn>",idc);
    else if (s=="g_")
      return tex?"g":mml_tag("mi","g",idc);
    else if (s=="G_")
      return tex?"G":mml_tag("mi","G",idc);
    else if (s=="h_")
      return tex?"h":mml_tag("mi","h",idc);
    else if (s=="hbar_")
      return tex?"\\hslash ":mml_tag("mi","&#x210f;",idc);
    else if (s=="I0_")
      return tex?"I_0":mml_tag("msub","<mi>I</mi><mn>0</mn>",idc);
    else if (s=="k_")
      return tex?"k":mml_tag("mi","k",idc);
    else if (s=="lambda0_")
      return tex?"\\lambda_0":mml_tag("msub","<mi>&lambda;</mi><mn>0</mn>",idc);
    else if (s=="lambdac_")
      return tex?"\\lambda ":mml_tag("mi","&lambda;",idc);
    else if (s=="me_")
      return tex?"m_\\mathrm{e}"
                :mml_tag("msub","<mi>m</mi><mi mathvariant='normal'>e</mi>",idc);
    else if (s=="mEarth_")
      return tex?"M_\\oplus "
                :mml_tag("msub","<mi>M</mi><mi>&oplus;</mi>",idc);
    else if (s=="mp_")
      return tex?"m_\\mathrm{p}"
                :mml_tag("msub","<mi>m</mi><mi mathvariant='normal'>p</mi>",idc);
    else if (s=="mu0_")
      return tex?"\\mu_0":mml_tag("msub","<mi>&mu;</mi><mn>0</mn>",idc);
    else if (s=="muB_")
      return tex?"\\mu_\\mathrm{B}"
                :mml_tag("msub","<mi>&mu;</mi><mi mathvariant='normal'>B</mi>",idc);
    else if (s=="muN_")
      return tex?"\\mu_\\mathrm{N}"
                :mml_tag("msub","<mi>&mu;</mi><mi mathvariant='normal'>N</mi>",idc);
    else if (s=="NA_")
      return tex?"N_A":mml_tag("msub","<mi>N</mi><mi>A</mi>",idc);
    else if (s=="phi_")
      return tex?"\\Phi_0":mml_tag("msub","<mi>&Phi;</mi><mn>0</mn>",idc);
    else if (s=="PSun_")
      return tex?"P_\\odot "
                :mml_tag("msub","<mi>P</mi><mi>&odot;</mi>",idc);
    else if (s=="q_")
      return tex?"q":mml_tag("mi","q",idc);
    else if (s=="R_")
      return tex?"R":mml_tag("mi","R",idc);
    else if (s=="REarth_")
      return tex?"R_\\oplus "
                :mml_tag("msub","<mi>R</mi><mi>&oplus;</mi>",idc);
    else if (s=="Rinfinity_")
      return tex?"R_\\infty "
                :mml_tag("msub","<mi>R</mi><mi>&infin;</mi>",idc);
    else if (s=="RSun_")
      return tex?"R_\\odot "
                :mml_tag("msub","<mi>R</mi><mi>&odot;</mi>",idc);
    else if (s=="sigma_")
      return tex?"\\sigma ":mml_tag("mi","&sigma;",idc);
    else if (s=="StdP_")
      return tex?"P_\\mathrm{std}"
                :mml_tag("msub","<mi>P</mi><mi>std</mi>",idc);
    else if (s=="StdT_")
      return tex?"T_\\mathrm{std}"
                :mml_tag("msub","<mi>T</mi><mi>std</mi>",idc);
    else if (s=="Vm_")
      return tex?"V_\\mathrm{m}"
                :mml_tag("msub","<mi>V</mi><mi mathvariant='normal'>m</mi>",idc);
    else if (s=="degreeF")
      return tex?"{}^\\circ\\mathrm{F}"
                :mml_tag("mi","&#x2109;",idc,"class","MathML-Unit","mathvariant","normal");
    else if (s=="ozfl")
      return tex?"\\mathrm{oz}_\\mathrm{fl}"
                :mml_tag("msub","<mi>oz</mi><mi>fl</mi>",idc,"class","MathML-Unit");
    else if (s=="fermi")
      return tex?"\\mathrm{fm}"
                :mml_tag("mi","fm",idc,"class","MathML-Unit");
    else if (s=="flam")
      return tex?"\\mathrm{fL}"
                :mml_tag("mi","fL",idc,"class","MathML-Unit");
    else if (s=="deg")
      return tex?"{}^\\circ "
                :mml_tag("mi","&#x00b0;",idc,"class","MathML-Unit");
    else if (s=="arcmin")
      return tex?"'":mml_tag("mi","&prime;",idc,"class","MathML-Unit");
    else if (s=="arcs")
      return tex?"''":mml_tag("mi","&Prime;",idc,"class","MathML-Unit");
    else if (s=="Rankine")
      return tex?"{}^\\circ\\mathrm{R}"
                :mml_tag("mi","&#x00b0;R",idc,"class","MathML-Unit");
    else if (s=="lam")
      return tex?"\\mathrm{L}"
                :mml_tag("mi","L",idc,"class","MathML-Unit","mathvariant","normal");
    else if (s=="inH2O")
      return tex?"\\mathrm{in}_\\mathrm{H_2O}"
                :mml_tag("msub","<mi>in</mi><mrow><msub><mn>H</mn><mn>2</mn></msub>"
                           "<mn>O</mn></mrow>",idc,"class","MathML-Unit");
    else if (s=="buUS")
      return tex?"\\mathrm{bu}_\\mathrm{US}"
                :mml_tag("msub","<mi>bu</mi><mi>US</mi>",idc,"class","MathML-Unit");
    else if (s=="ftUS")
      return tex?"\\mathrm{ft}_\\mathrm{US}"
                :mml_tag("msub","<mi>ft</mi><mi>US</mi>",idc,"class","MathML-Unit");
    else if (s=="galC")
      return tex?"\\mathrm{gal}_\\mathrm{C}"
                :mml_tag("msub","<mi>gal</mi><mi>C</mi>",idc,"class","MathML-Unit");
    else if (s=="galUK")
      return tex?"\\mathrm{gal}_\\mathrm{UK}"
                :mml_tag("msub","<mi>gal</mi><mi>UK</mi>",idc,"class","MathML-Unit");
    else if (s=="galUS")
      return tex?"\\mathrm{gal}_\\mathrm{US}"
                :mml_tag("msub","<mi>gal</mi><mi>US</mi>",idc,"class","MathML-Unit");
    else if (s=="inHg")
      return tex?"\\mathrm{in}_\\mathrm{Hg}"
                :mml_tag("msub","<mi>in</mi><mi>Hg</mi>",idc,"class","MathML-Unit");
    else if (s=="miUS")
      return tex?"\\mathrm{mi}_\\mathrm{US}"
                :mml_tag("msub","<mi>mi</mi><mi>US</mi>",idc,"class","MathML-Unit");
    else if (s=="mmHg")
      return tex?"\\mathrm{mm}_\\mathrm{Hg}"
                :mml_tag("msub","<mi>mm</mi><mi>Hg</mi>",idc,"class","MathML-Unit");
    else if (s=="ozUK")
      return tex?"\\mathrm{oz}_\\mathrm{UK}"
                :mml_tag("msub","<mi>oz</mi><mi>UK</mi>",idc,"class","MathML-Unit");
    else if (s=="ptUK")
      return tex?"\\mathrm{pt}_\\mathrm{UK}"
                :mml_tag("msub","<mi>pt</mi><mi>UK</mi>",idc,"class","MathML-Unit");
    else if (s=="tonUK")
      return tex?"\\mathrm{ton}_\\mathrm{UK}"
                :mml_tag("msub","<mi>ton</mi><mi>UK</mi>",idc,"class","MathML-Unit");
    else if (!s.empty() && s[s.size()-1]!='_') {
      string p;
      if (s.substr(0,2)=="µ") {
        p=tex?"\\mu ":"&mu;";
        s=s.substr(2);
      }
      if (s=="Angstrom")
        return tex?"\\mathrm{"+p+"\\AA}"
                  :mml_tag("mi",p+"&#x00c5;",idc,"class","MathML-Unit");
      else if (s.substr(1)=="Angstrom") {
        string pr(s.substr(0,1));
        return tex?"\\mathrm{"+pr+"\\AA}"
                  :mml_tag("mi",pr+"&#x00c5;",idc,"class","MathML-Unit");
      } else if (s=="Ohm")
        return tex?p+"\\Omega "
                  :mml_tag("mi",p+"&Omega;",idc,"class","MathML-Unit");
      else if (s.substr(1)=="Ohm") {
        string pr(s.substr(0,1));
        return tex?"\\mathrm{"+pr+"}\\Omega "
                  :mml_tag("mi",pr+"&Omega;",idc,"class","MathML-Unit");
      } else
        return tex?"\\mathrm{"+p+s+"}"
                  :mml_tag("mi",p+s,idc,(p+s).length()<2?" mathvariant":"","normal");
    }
  }
  /*
  if (s_orig=="i")
    return tex?"\\mathrm{i}":insert_id(mml_i,idc);
  */
  size_t i,len,len_sub;
  string s,ssub,mdf="";
  if (s_orig.rfind("_")==s_orig.size()-1 && is_greek_letter(s=s_orig.substr(0,s_orig.size()-1)))
    return tex?"\\"+s+"\\_":"&"+s+";_";
  for (i=s_orig.size();i-->0;) {
    if (!isdigit(s_orig[i]))
      break;
  }
  s=s_orig.substr(0,i+1);
  if (i<s_orig.size()-1 && (s=="log" || s.size()==1 || is_greek_letter(s) || is_double_letter(s)))
    ssub=s_orig.substr(i+1);
  if (ssub.empty()) {
    size_t pos=s_orig.find("_");
    if (pos!=string::npos) {
      s=s_orig.substr(0,pos);
      if (s.size()==1 || is_greek_letter(s) || is_double_letter(s))
        ssub=s_orig.substr(pos+1);
    }
  }
  if (ssub.empty())
    s=s_orig;
  len=s.size();
  if (is_greek_letter(s)) {
    if (tex && s=="phi")
      s="varphi";
    s=tex?("\\"+s):("&"+s+";");
    len=1;
  }
  len_sub=ssub.size();
  if (!ssub.empty() && is_greek_letter(ssub)) {
    ssub=tex?("\\"+s):("&"+ssub+";");
    len_sub=1;
  }
  if (is_double_letter(s)) {
    s=s.substr(0,1);
    if (tex) {
      s="\\mathbf{"+s+"}";
      len=1;
    } else
      mdf="mathvariant";
  }
  if (len>3 && s[0]=='`' && s.at(1)==32 && s[s.size()-1]=='`')
    s=s.substr(2,len-3);
  string ret=(len==1?(tex?s:mml_tag("mi",s,ssub.empty()?idc:0))
                    :(tex?"\\mathrm{"+s+"}":mml_tag("mi",s,ssub.empty()?idc:0,mdf,"bold")));
  if (!ssub.empty()) {
    if (tex)
      ret+="_{"+(len_sub==1 && !isdigit(ssub[0])?ssub:"\\mathrm{"+ssub+"}")+"}";
    else
      ret=mml_tag("msub",ret+(ssub=="0" || atof(ssub.c_str())!=0?
                         "<mn>"+ssub+"</mn>":"<mi>"+ssub+"</mi>"),idc);
  } else if (tex) {
    int i=0;
    while (i<(int)ret.length()) {
      if (ret.at(i)=='_') {
        ret.replace(i,1,"\\_");
        i+=2;
      } else ++i;
    }
  }
  return ret;
}

vecteur flatten_operands(const gen &g) {
  assert(g.type==_SYMB);
  vecteur ops;
  gen &arg=g._SYMBptr->feuille;
  if (arg.type!=_VECT)
    ops.push_back(arg);
  else {
    vecteur &args=*arg._VECTptr;
    for (int i=0;i<int(args.size());++i) {
      gen op=args[i];
      const_iterateur it;
      if (op.type==_SYMB && op._SYMBptr->sommet==g._SYMBptr->sommet) {
        vecteur subops=flatten_operands(op);
        for (it=subops.begin();it!=subops.end();++it)
          ops.push_back(*it);
      } else
        ops.push_back(op);
    }
  }
  return ops;
}

void parenthesize(MarkupBlock &ml,int flags) {
  if ((flags & _MARKUP_LATEX)!=0)
    ml.latex="\\left("+ml.latex+"\\right)";
  if ((flags & _MARKUP_MATHML_PRESENTATION)!=0)
    ml.markup="<mfenced>"+ml.markup+"</mfenced>";
  ml.priority=0;
}

void prepend_minus(MarkupBlock &ml,int flags,bool circled=false,bool mrow=true) {
  if (!ml.neg)
    return;
  string id;
  if (ml.priority>_PRIORITY_MUL)
    parenthesize(ml,flags);
  if ((flags & _MARKUP_LATEX)!=0)
    ml.latex=(circled?"\\ominus ":"-")+ml.latex;
  if ((flags & _MARKUP_MATHML_CONTENT)!=0) {
    if (mrow) {
      id=extract_id(ml,true);
      ml.content=mml_tag("apply","<minus/>"+ml.content,0,id.empty()?"":"id",id);
    } else
      ml.content="<apply><minus/>"+ml.content+"</apply>";
  }
  if ((flags & _MARKUP_MATHML_PRESENTATION)!=0) {
    if (mrow) {
      id=extract_id(ml,false);
      ml.markup=mml_tag("mrow",(circled?"<mo>&ominus;</mo>":mml_minus)+ml.markup,
                        0,id.empty()?"":"xref",id);
    } else ml.markup=(circled?"<mo>&ominus;</mo>":mml_minus)+ml.markup;
  }
  ml.neg=false;
  ml.priority=_PRIORITY_MUL;
  ml.type=_MLBLOCK_NEG;
}

void assoc2markup(const vecteur &args,MarkupBlock &ml,const string &op,
                  const string &opc,const string &opt,int flags,int &idc,
                  GIAC_CONTEXT) {
  MarkupBlock tmp;
  bool tex=(flags & _MARKUP_LATEX)!=0,
       mml=(flags & _MARKUP_MATHML_PRESENTATION)!=0;
  bool cont=(flags & _MARKUP_MATHML_CONTENT)!=0;
  for (const_iterateur it=args.begin();it!=args.end();++it) {
    tmp=gen2markup(*it,flags,idc,contextptr);
    prepend_minus(tmp,flags);
    if ((tex || mml) && tmp.priority>=ml.priority)
      parenthesize(tmp,flags);
    if (it!=args.begin()) {
      if (mml)
        ml.markup+="<mo>"+op+"</mo>";
      if (tex)
        ml.latex+=opt;
    }
    if (cont)
      ml.content+=tmp.content;
    if (mml)
      ml.markup+=tmp.markup;
    if (tex)
      ml.latex+=tmp.latex;
  }
  if (cont)
    ml.content=mml_tag("apply",opc+ml.content,++idc);
  if (mml)
    ml.markup=mml_tag("mrow",ml.markup,idc);
}

void get_leftright(const gen &arg,MarkupBlock *ml,MarkupBlock &left,
                   MarkupBlock &right,int flags,int &idc,GIAC_CONTEXT) {
  assert(arg.type==_VECT);
  vecteur &args=*arg._VECTptr;
  left=gen2markup(args.front(),flags,idc,contextptr);
  prepend_minus(left,flags);
  if (ml!=NULL && left.priority>=ml->priority)
    parenthesize(left,flags);
  right=gen2markup(args.back(),flags,idc,contextptr);
  prepend_minus(right,flags);
  if (ml!=NULL && right.priority>=ml->priority)
    parenthesize(right,flags);
}

bool is_partialdiff(const gen &g,identificateur &f,vecteur &deg,GIAC_CONTEXT) {
    gen df=g;
    deg.clear();
    while (df.is_symb_of_sommet(at_derive) && df._SYMBptr->feuille.type==_VECT) {
        const vecteur &args=*df._SYMBptr->feuille._VECTptr;
        if (args.size()!=2 || !args.back().is_integer() || !is_positive(args.back(),contextptr))
            return false;
        deg.push_back(args.back()+1);
        df=args.front();
    }
    if (df.type!=_IDNT || deg.empty()) return false;
    f=*df._IDNTptr;
    std::reverse(deg.begin(),deg.end());
    return true;
}

bool get_derive_vars(const vecteur &args,vecteur &vars) {
  int n=args.size(),k;
  if (n==3 && args[1].type==_IDNT && args.back().is_integer() &&
      (k=args.back().val)>0) {
    if (!vars.empty() && vars.back()._VECTptr->front()==args[1])
      vars.back()._VECTptr->back()+=k;
    else vars.push_back(makevecteur(args[1],k));
    return true;
  }
  for (int i=1;i<n;++i) {
    const gen &g=args[i];
    if (g.is_symb_of_sommet(at_dollar)) {
      const gen &h=g._SYMBptr->feuille._VECTptr->front();
      const gen &m=g._SYMBptr->feuille._VECTptr->back();
      if (h.type!=_IDNT || !m.is_integer() || m.val<=0)
        return false;
      if (!vars.empty() && vars.back()._VECTptr->front()==h)
        vars.back()._VECTptr->back()+=m;
      vars.push_back(makevecteur(h,m));
    } else
      switch (g.type) {
      case _IDNT:
        if (!vars.empty() && vars.back()._VECTptr->front()==g)
          vars.back()._VECTptr->back()+=1;
        vars.push_back(makevecteur(g,1));
        break;
      case _VECT:
        if (!get_derive_vars(*g._VECTptr,vars))
          return false;
        break;
      default:
        return false;
      }
  }
  return true;
}

bool is_elemfunc(const gen &g) {
  return g.is_symb_of_sommet(at_ln) || g.is_symb_of_sommet(at_LN) ||
         g.is_symb_of_sommet(at_sin) || g.is_symb_of_sommet(at_SIN) ||
         g.is_symb_of_sommet(at_cos) || g.is_symb_of_sommet(at_COS) ||
         g.is_symb_of_sommet(at_sec) || g.is_symb_of_sommet(at_SEC) ||
         g.is_symb_of_sommet(at_csc) || g.is_symb_of_sommet(at_CSC) ||
         g.is_symb_of_sommet(at_tan) || g.is_symb_of_sommet(at_TAN) ||
         g.is_symb_of_sommet(at_cot) || g.is_symb_of_sommet(at_COT) ||
         g.is_symb_of_sommet(at_asec) || g.is_symb_of_sommet(at_ASEC) ||
         g.is_symb_of_sommet(at_acsc) || g.is_symb_of_sommet(at_ACSC) ||
         g.is_symb_of_sommet(at_asin) || g.is_symb_of_sommet(at_ASIN) ||
         g.is_symb_of_sommet(at_acos) || g.is_symb_of_sommet(at_ACOS) ||
         g.is_symb_of_sommet(at_atan) || g.is_symb_of_sommet(at_ATAN) ||
         g.is_symb_of_sommet(at_acot) || g.is_symb_of_sommet(at_ACOT) ||
         g.is_symb_of_sommet(at_sinh) || g.is_symb_of_sommet(at_SINH) ||
         g.is_symb_of_sommet(at_cosh) || g.is_symb_of_sommet(at_COSH) ||
         g.is_symb_of_sommet(at_tanh) || g.is_symb_of_sommet(at_TANH) ||
         g.is_symb_of_sommet(at_asinh) || g.is_symb_of_sommet(at_ASINH) ||
         g.is_symb_of_sommet(at_acosh) || g.is_symb_of_sommet(at_ACOSH) ||
         g.is_symb_of_sommet(at_atanh) || g.is_symb_of_sommet(at_ATANH);
}

string func2markup(const gen &g,bool tex,bool content,int idc=0) {
  string ret;
  bool has_id=false;
  if (g.is_symb_of_sommet(at_ln) || g.is_symb_of_sommet(at_LN))
    ret=tex?"\\ln ":(content? "<ln/>":"<mi>ln</mi>");
  else if (g.is_symb_of_sommet(at_sin) || g.is_symb_of_sommet(at_SIN))
    ret=tex?"\\sin ":(content? "<sin/>":"<mi>sin</mi>");
  else if (g.is_symb_of_sommet(at_cos) || g.is_symb_of_sommet(at_COS))
    ret=tex?"\\cos ":(content? "<cos/>":"<mi>cos</mi>");
  else if (g.is_symb_of_sommet(at_tan) || g.is_symb_of_sommet(at_TAN))
    ret=tex?"\\tan ":(content? "<tan/>":"<mi>tan</mi>");
  else if (g.is_symb_of_sommet(at_cot) || g.is_symb_of_sommet(at_COT))
    ret=tex?"\\cot ":(content? "<cot/>":"<mi>cot</mi>");
  else if (g.is_symb_of_sommet(at_sinh) || g.is_symb_of_sommet(at_SINH))
    ret=tex?"\\sinh ":(content? "<sinh/>":"<mi>sinh</mi>");
  else if (g.is_symb_of_sommet(at_cosh) || g.is_symb_of_sommet(at_COSH))
    ret=tex?"\\cosh ":(content? "<cosh/>":"<mi>cosh</mi>");
  else if (g.is_symb_of_sommet(at_tanh) || g.is_symb_of_sommet(at_TANH))
    ret=tex?"\\tanh ":(content? "<tanh/>":"<mi>tanh</mi>");
  else if (g.is_symb_of_sommet(at_asin) || g.is_symb_of_sommet(at_ASIN))
    ret=tex?"\\arcsin ":(content? "<arcsin/>":"<mi>arcsin</mi>");
  else if (g.is_symb_of_sommet(at_acos) || g.is_symb_of_sommet(at_ACOS))
    ret=tex?"\\arccos ":(content? "<arccos/>":"<mi>arccos</mi>");
  else if (g.is_symb_of_sommet(at_atan) || g.is_symb_of_sommet(at_ATAN))
    ret=tex?"\\arctan ":(content? "<arctan/>":"<mi>arctan</mi>");
  else if (g.is_symb_of_sommet(at_acot) || g.is_symb_of_sommet(at_ACOT))
    ret=tex?"\\operatorname{\\mathrm{arccot} }":(content? "<arccot/>":"<mi>arccot</mi>");
  else if (g.is_symb_of_sommet(at_sec) || g.is_symb_of_sommet(at_SEC))
    ret=tex?"\\sec ":(content? "<sec/>":"<mi>sec</mi>");
  else if (g.is_symb_of_sommet(at_csc) || g.is_symb_of_sommet(at_CSC))
    ret=tex?"\\csc ":(content? "<csc/>":"<mi>csc</mi>");
  else if (g.is_symb_of_sommet(at_asec) || g.is_symb_of_sommet(at_ASEC))
    ret=tex?"\\operatorname{\\mathrm{arcsec} }":(content? "<arcsec/>":"<mi>arcsec</mi>");
  else if (g.is_symb_of_sommet(at_acsc) || g.is_symb_of_sommet(at_ACSC))
    ret=tex?"\\operatorname{\\mathrm{arccsc} }":(content? "<arccsc/>":"<mi>arccsc</mi>");
  else if (g.is_symb_of_sommet(at_asinh) || g.is_symb_of_sommet(at_ASINH))
    ret=tex?"\\operatorname{\\mathrm{arsinh} }":(content? "<arcsinh/>":"<mi>arsinh</mi>");
  else if (g.is_symb_of_sommet(at_acosh) || g.is_symb_of_sommet(at_ACOSH))
    ret=tex?"\\operatorname{\\mathrm{arcosh} }":(content? "<arccosh/>":"<mi>arcosh</mi>");
  else if (g.is_symb_of_sommet(at_atanh) || g.is_symb_of_sommet(at_ATANH))
    ret=tex?"\\operatorname{\\mathrm{artanh} }":(content? "<arctanh/>":"<mi>artanh</mi>");
  else {
    if (g.is_symb_of_sommet(at_id) && content) ret="<ident/>";
    else if (g.is_symb_of_sommet(at_gcd) && content) ret="<gcd/>";
    else if (g.is_symb_of_sommet(at_lcm) && content) ret="<lcm/>";
    else if ((g.is_symb_of_sommet(at_irem) || g.is_symb_of_sommet(at_rem)) && content) ret="<rem/>";
    else if ((g.is_symb_of_sommet(at_iquo) || g.is_symb_of_sommet(at_quo)) && content) ret="<quotient/>";
    else if (g.is_symb_of_sommet(at_arg) && content) ret="<arg/>";
    else if (g.is_symb_of_sommet(at_max) && content) ret="<max/>";
    else if (g.is_symb_of_sommet(at_min) && content) ret="<min/>";
    else if (g.is_symb_of_sommet(at_det) && content) ret="<determinant/>";
    else if (g.is_symb_of_sommet(at_Dirac) && !content) ret=tex?"\\delta":"<mi>&delta;</mi>";
    else if (g.is_symb_of_sommet(at_Heaviside) && !content) ret=tex?"\\Theta":"<mi>&Theta;</mi>";
    else if (g.is_symb_of_sommet(at_Gamma))
      ret=tex?"\\Gamma":(content?mml_csymbol("gamma","hypergeo0")
                                       :"<mi mathvariant='normal'>&Gamma;</mi>");
    else if (g.is_symb_of_sommet(at_Beta))
      ret=tex?"\\Beta":(content?mml_csymbol("beta","hypergeo0")
                                     :"<mi mathvariant='normal'>&Beta;</mi>");
    else if (g.is_symb_of_sommet(at_euler))
      ret=tex?"\\phi":(content?mml_csymbol("euler","integer2")
                                     :"<mi mathvariant='normal'>&phi;</mi>");
    else if (g.is_symb_of_sommet(at_Airy_Ai))
      ret=tex?"\\mathrm{Ai}":(content?mml_csymbol("Ai","airy"):"<mi>Ai</mi>");
    else if (g.is_symb_of_sommet(at_Airy_Bi))
      ret=tex?"\\mathrm{Bi}":(content?mml_csymbol("Bi","airy"):"<mi>Bi</mi>");
    else if (g.is_symb_of_sommet(at_Psi) && !content)
      ret=tex?"\\Psi":"<mi mathvariant='normal'>&Psi;</mi>";
    else if (g.is_symb_of_sommet(at_Zeta) && !content)
      ret=tex?"\\zeta":"<mi mathvariant='normal'>&zeta;</mi>";
    else {
      if (!tex && content)
        ret=mml_tag("ci",g._SYMBptr->sommet.ptr()->s,idc,"type","function");
      else
        ret=idnt2markup(g._SYMBptr->sommet.ptr()->s,tex,false,idc);
      has_id=true;
    }
    if (tex)
      ret="\\operatorname{"+ret+" }";
  }
  if (!tex && !has_id)
    ret=insert_id(ret,idc,content);
  return ret;
}

bool is_set_or_ident(const gen &g) {
  return (g.type==_VECT && g.subtype==_SEQ__VECT) || g.type==_IDNT ||
         (g.type==_INT_ && g.subtype==_INT_TYPE && (g.val==_ZINT || g.val==_CPLX ||
                                                        g.val==_DOUBLE_ || g.val==_FRAC));
}

MarkupBlock gen2markup(const gen &g,int flags_orig,int &idc,GIAC_CONTEXT) {
  int flags=flags_orig;
  bool toplevel=(flags & _MARKUP_TOPLEVEL)!=0;
  bool tex=(flags & _MARKUP_LATEX)!=0;
  bool isunit=(flags & _MARKUP_UNIT)!=0;
  bool mml_presentation=(flags & _MARKUP_MATHML_PRESENTATION)!=0;
  bool mml_content=(flags & _MARKUP_MATHML_CONTENT)!=0;
  bool isfactor=(flags & _MARKUP_FACTOR)!=0;
  bool vectarg,isbinary,isone;
  flags &= ~_MARKUP_TOPLEVEL;
  flags &= ~_MARKUP_FACTOR;
  string ld,rd,ld_tex,rd_tex,str;
  gen h;
  const gen *gp;
  MarkupBlock ml,tmp,left,right;
  size_t cpos;
  ml.priority=0;
  ml.type=_MLBLOCK_GENERAL;
  ml.neg=ml.appl=false;
  ml.markup.clear();
  ml.split_pos=-1;
  int st=g.subtype;
  switch (g.type) {
  case _INT_:
  case _ZINT:
    if (g.type==_INT_ && g.subtype!=0) {
      bool success=false;
      if (g.subtype==_INT_TYPE) {
        success=g.val==_ZINT || g.val==_FRAC || g.val==_CPLX || g.val==_DOUBLE_;
        if (success && mml_content)
          switch (g.val) {
          case _ZINT:
            ml.content=insert_id("<integers/>",++idc,true);
            break;
          case _FRAC:
            ml.content=insert_id("<rationals/>",++idc,true);
            break;
          case _CPLX:
            ml.content=insert_id("<complexes/>",++idc,true);
            break;
          case _DOUBLE_:
            ml.content=insert_id("<reals/>",++idc,true);
            break;
          }
        if (success && mml_presentation)
          switch (g.val) {
          case _ZINT:
            ml.markup=mml_tag("mi","&integers;",idc);
            break;
          case _FRAC:
            ml.markup=mml_tag("mi","&rationals;",idc);
            break;
          case _CPLX:
            ml.markup=mml_tag("mi","&complexes;",idc);
            break;
          case _DOUBLE_:
            ml.markup=mml_tag("mi","&reals;",idc);
            break;
          }
        if (success && tex)
          switch (g.val) {
          case _ZINT:
            ml.latex="\\mathbb{Z}";
            break;
          case _FRAC:
            ml.latex="\\mathbb{Q}";
            break;
          case _CPLX:
            ml.latex="\\mathbb{C}";
            break;
          case _DOUBLE_:
            ml.latex="\\mathbb{R}";
            break;
          }
      }
      if (!success) {
        if (mml_content) {
          if (g.type==_INT_ && g.subtype==_INT_BOOLEAN)
            ml.content=insert_id((bool)g.val?"<true>":"<false>",++idc,true);
          else
            ml.content=mml_tag("ci",g.print(contextptr),++idc);
        }
        if (mml_presentation)
          ml.markup=idnt2markup(g.print(contextptr),false,false,idc);
        if (tex)
          ml.latex=idnt2markup(g.print(contextptr),true,false);
        ml.type=_MLBLOCK_SUBTYPE_IDNT;
      }
    } else {
      ml.type=_MLBLOCK_NUMERIC_EXACT;
      ml.neg=!is_positive(g,contextptr);
      str=_abs(g,contextptr).print(contextptr);
      if (mml_content)
        ml.content=mml_tag("cn",str,++idc,"type","integer");
      if (mml_presentation)
        ml.markup=mml_tag("mn",str,idc);
      if (tex)
        ml.latex=str;
    }
    return ml;
  case _DOUBLE_:
  case _REAL:
  case _FLOAT_:
    ml.type=_MLBLOCK_NUMERIC_APPROX;
    ml.neg=!is_positive(g,contextptr);
    if (isunit && is_zero(fPart(g,contextptr))) {
      ml.type=_MLBLOCK_NUMERIC_EXACT;
      str=_round(_abs(g,contextptr),contextptr).print(contextptr);
      if (mml_content)
        ml.content=mml_tag("cn",str,++idc,"type","integer");
      if (mml_presentation)
        ml.markup=mml_tag("mn",str,idc);
      if (tex)
        ml.latex=str;
    } else {
      str=_abs(g,contextptr).print(contextptr);
      if (mml_content)
        ml.content=mml_tag("cn",str,++idc,"type","double");
      if ((cpos=str.find('e'))!=string::npos &&
          atof(str.substr(0,cpos).c_str())!=0 && atof(str.substr(cpos+2).c_str())!=0) {
        ml.priority=_PRIORITY_MUL;
        string ex=str.substr(cpos+2);
        while (ex[0]=='0')
          ex.erase(ex.begin());
        if (mml_presentation)
          ml.markup=mml_tag("mrow","<mn>"+str.substr(0,cpos)+
                            "</mn><mo>&times;</mo><msup><mn>10</mn><mrow>"+
                            string(str[cpos+1]=='-'?mml_minus:"")+"<mn>"+ex+"</mn></mrow></msup>",idc);
        if (tex)
          ml.latex=str.substr(0,cpos)+"\\times10^{"+string(str[cpos+1]=='-'?"-":"")+ex+"}";
      } else {
        if (mml_presentation)
          ml.markup=mml_tag("mn",str,idc);
        if (tex)
          ml.latex=str;
      }
    }
    return ml;
  case _CPLX:
    if (is_zero(im(g,contextptr)))
      return gen2markup(re(g,contextptr),flags,idc,contextptr);
    isone=is_one(_abs(im(g,contextptr),contextptr));
    if (is_zero(re(g,contextptr))) {
      if (isone) {
        if (mml_content)
          ml.content=insert_id("<imaginaryi/>",idc,true);
        if (mml_presentation)
          ml.markup=insert_id(mml_i,idc,false);
        if (tex)
          ml.latex="\\mathrm{i}";
        ml.neg=is_minus_one(im(g,contextptr));
      } else {
        tmp=gen2markup(im(g,contextptr),flags | _MARKUP_FACTOR,idc,contextptr);
        ml.neg=tmp.neg;
        ml.priority=_PRIORITY_MUL;
        if (tmp.priority>=ml.priority)
          parenthesize(tmp,flags);
        if (mml_content)
          ml.content=mml_tag("apply","<times/><imaginaryi/>"+tmp.markup,++idc);
        if (mml_presentation)
          ml.markup=mml_tag("mrow",tmp.markup+mml_itimes+mml_i,idc);
        if (tex)
          ml.latex=tmp.latex+"\\,\\mathrm{i}";
      }
      return ml;
    }
    ml.priority=_PRIORITY_ADD;
    left=gen2markup(re(g,contextptr),flags,idc,contextptr);
    prepend_minus(left,flags);
    if (left.priority>=ml.priority)
      parenthesize(left,flags);
    right=gen2markup(im(g,contextptr),flags,idc,contextptr);
    if (right.priority>=_PRIORITY_MUL)
      parenthesize(right,flags);
    if (mml_content)
      ml.content=mml_tag("apply",(right.neg?"<minus/>":"<plus/>")+left.content+
                                 (isone?"<imaginaryi/>"
                                       :mml_tag("apply","<times/><imaginaryi/>"+right.content)),
                         ++idc);
    if (mml_presentation)
      ml.markup=mml_tag("mrow",left.markup+(right.neg?mml_minus:mml_plus)+
                               (isone?"":right.markup+mml_itimes)+mml_i,
                        idc);
    if (tex)
      ml.latex=left.latex+(right.neg?"-":"+")+(isone?"":right.latex+"\\,")+"\\mathrm{i}";
    return ml;
  case _FRAC:
    ml.type=_MLBLOCK_FRACTION;
    ml.priority=_PRIORITY_MUL;
    if (!is_zero(im(g._FRACptr->num,contextptr)))
      return gen2markup(symbolic(at_division,makesequence(g._FRACptr->num,g._FRACptr->den)),
                        flags,idc,contextptr);
    ml.neg=!is_positive(g,contextptr);
    ld=_abs(g._FRACptr->num,contextptr).print(contextptr);
    rd=_abs(g._FRACptr->den,contextptr).print(contextptr);
    if (mml_content)
      ml.content=mml_tag("cn",ld+"<sep/>"+rd,++idc,"type","rational");
    if (mml_presentation)
      ml.markup=mml_tag("mfrac","<mn>"+ld+"</mn><mn>"+rd+"</mn>",idc);
    if (tex)
      ml.latex="\\frac{"+ld+"}{"+rd+"}";
    return ml;
  case _STRNG:
    if (g.subtype==-1)
      flags|=_MARKUP_ERROR;
    string2markup(ml,*g._STRNGptr,flags | _MARKUP_QUOTE,idc);
    return ml;
  case _IDNT:
    if (is_inf(g)) {
      if (mml_content)
        ml.content=insert_id("<infinity/>",++idc,true);
      if (mml_presentation)
        ml.markup=mml_tag("mi","&infin;",idc);
      if (tex)
        ml.latex="\\infty ";
    } else if (g==cst_pi) {
      if (mml_content)
        ml.content=insert_id("<pi/>",++idc,true);
      if (mml_presentation)
        ml.markup=mml_tag("mi","&pi;",idc);
      if (tex)
        ml.latex="\\pi ";
    } else if (g==cst_euler_gamma) {
      if (mml_content)
        ml.content=insert_id("<eulergamma/>",++idc,true);
      if (mml_presentation)
        ml.markup=mml_tag("mi","&gamma;",idc);
      if (tex)
        ml.latex="\\gamma ";
    } else {
      if (mml_content) {
        str=g.print(contextptr);
        if (isunit && str.length()>2 && str[0]=='_' && str[str.size()-1]=='_')
          ml.content=constant2content(str.substr(1,str.length()-2),++idc);
        else if (isunit && str.length()>1 && str[0]=='_')
          ml.content=unit2content(str.substr(1),++idc);
        else
          ml.content=mml_tag("ci",str,++idc);
      }
      bool has_subscript=false;
      if (mml_presentation) {
        ml.markup=idnt2markup(g.print(contextptr),false,isunit,idc);
        has_subscript=is_substr(ml.markup,"<msub>");
      }
      if (tex) {
        ml.latex=idnt2markup(g.print(contextptr),true,isunit);
        has_subscript=is_substr(ml.latex,"_{");
      }
      if (has_subscript)
        ml.type|=_MLBLOCK_HAS_SUBSCRIPT;
    }
    return ml;
  /*
  case _SPOL1:
    return gen2markup(spol12gen(*g._SPOL1ptr,contextptr),flags,idc,contextptr);
  */
  case _MAP:
    if (mml_presentation)
      ml.markup="<mtr style='background:lightgray'><mtd><mtext "
                  "mathvariant='bold'>key</mtext></mtd><mtd><mtext "
                  "mathvariant='bold'>value</mtext></mtd></mtr>";
    if (tex)
      ml.latex="\\begin{array}{|c|c|}\\hline\\mathbf{key}&\\mathbf{value}\\\\\\hline ";
    for (gen_map::const_iterator it=g._MAPptr->begin();
         it!=g._MAPptr->end();++it) {
      get_leftright(makevecteur(it->first,it->second),NULL,left,right,flags,idc,contextptr);
      if (mml_content)
        ml.content+=mml_tag("list",left.content+right.content);
      if (mml_presentation)
        ml.markup+="<mtr><mtd>"+left.markup+"</mtd><mtd>"+right.markup+"</mtd></mtr>";
      if (tex)
        ml.latex+=left.latex+"&"+right.latex+"\\\\\\hline ";
    }
    if (mml_content)
      ml.content=mml_tag("apply",mml_tag("ci","table")+ml.content,++idc);
    if (mml_presentation)
      ml.markup=mml_tag("mtable",ml.markup,idc,"frame","solid","rowlines",
                          "solid","columnlines","solid","rowalign","center");
    if (tex)
      ml.latex+="\\end{array}";
    return ml;
  case _MOD:
    ml.priority=_PRIORITY_MUL;
    get_leftright(makevecteur(*g._MODptr,*(g._MODptr+1)),
                  &ml,left,right,flags,idc,contextptr);
    if (mml_content)
      ml.content=mml_tag("apply","<ci>%</ci>"+left.content+right.content,++idc);
    if (mml_presentation)
      ml.markup=mml_tag("mrow",left.markup+"<mo>%</mo>"+right.markup,idc);
    if (tex)
      ml.latex=left.latex+"\\mathbin{\\%}"+right.latex;
    return ml;
  case _FUNC:
    if (mml_content)
      ml.content=func2markup(*g._FUNCptr,false,true,++idc);
    if (mml_presentation)
      ml.markup=func2markup(*g._FUNCptr,false,false,idc);
    if (tex)
      ml.latex=func2markup(*g._FUNCptr,true,false);
    ml.type=_MLBLOCK_FUNC;
    return ml;
  case _VECT:
    if (st==_GRAPH__VECT && is_graphe(g,str,contextptr)) {
      if (mml_content)
        ml.content=mml_tag("cs",str,++idc);
      if (mml_presentation)
        ml.markup=mml_tag("mtext",str,idc);
      if (tex)
        ml.latex="\\text{"+str+"}";
      return ml;
    }
    if (ckmatrix(*g._VECTptr) && (st==0 || st==_MATRIX__VECT)) {
      ml.type=_MLBLOCK_MATRIX;
      for (const_iterateur it=g._VECTptr->begin();it!=g._VECTptr->end();++it) {
        if (mml_content)
          ml.content+="<matrixrow>";
        if (mml_presentation)
          ml.markup+="<mtr>";
        if (tex)
          ml.latex+=(it==g._VECTptr->begin()?"":"\\\\");
        for (const_iterateur jt=it->_VECTptr->begin();jt!=it->_VECTptr->end();++jt) {
          tmp=gen2markup(*jt,flags,idc,contextptr);
          prepend_minus(tmp,flags);
          if (mml_content)
            ml.content+=tmp.content;
          if (mml_presentation)
            ml.markup+="<mtd>"+tmp.markup+"</mtd>";
          if (tex)
            ml.latex+=(jt==it->_VECTptr->begin()?"":"&")+tmp.latex;
        }
        if (mml_content)
          ml.content+="</matrixrow>";
        if (mml_presentation)
          ml.markup+="</mtr>";
      }
      if (mml_content)
        ml.content=mml_tag("matrix",ml.content,++idc);
      if (st==0) {
        ld="[";
        rd="]";
      } else {
        ld="(";
        rd=")";
      }
      if (mml_presentation)
        ml.markup=mml_tag("mfenced","<mtable>"+ml.markup+"</mtable>",
                            idc,"open",ld,"close",rd);
      if (tex)
        ml.latex="\\left"+ld+"\\begin{array}{"+string(g._VECTptr->front()._VECTptr->size(),'c')+"}"+
                   ml.latex+"\\end{array}\\right"+rd;
      return ml;
    }
    for (const_iterateur it=g._VECTptr->begin();it!=g._VECTptr->end();++it) {
      tmp=gen2markup(*it,flags,idc,contextptr);
      prepend_minus(tmp,flags);
      if (mml_content)
        ml.content+=tmp.content;
      if (mml_presentation)
        ml.markup+=tmp.markup;
      if (tex)
        ml.latex+=(it==g._VECTptr->begin()?"":",")+tmp.latex;
    }
    switch (st) {
    case _SEQ__VECT:
      if (mml_content)
        ml.content=mml_tag("list",ml.content,++idc);
      ld_tex=ld=toplevel?"":"(";
      rd_tex=rd=toplevel?"":")";
      break;
    case _SET__VECT:
      if (g._VECTptr->empty()) {
        if (mml_content)
          ml.content=insert_id("<emptyset/>",++idc,true);
        if (mml_presentation)
          ml.markup=mml_tag("mi","&empty;",idc);
        if (tex)
          ml.latex="\\emptyset ";
        return ml;
      }
      if (mml_content)
        ml.content=mml_tag("set",ml.content,++idc);
      ld="{";
      rd="}";
      ld_tex="\\{";
      rd_tex="\\}";
      break;
    default:
      if (mml_content)
        ml.content=mml_tag("vector",ml.content,++idc);
      ld_tex=ld="[";
      rd_tex=rd="]";
      break;
    }
    if (mml_presentation)
      ml.markup=mml_tag("mfenced",ml.markup,idc,"open",ld,"close",rd);
    if (tex)
      ml.latex=(ld_tex.empty()?"":"\\left"+ld_tex)+ml.latex+(rd_tex.empty()?"":"\\right"+rd_tex);
    return ml;
  case _SYMB:
    vectarg=g._SYMBptr->feuille.type==_VECT;
    isbinary=(vectarg && g._SYMBptr->feuille._VECTptr->size()==2);
    if (g.is_symb_of_sommet(at_neg)) {
      ml=gen2markup(g._SYMBptr->feuille,flags,idc,contextptr);
      ml.neg=!ml.neg;
      return ml;
    }
    if (g.is_symb_of_sommet(at_plus) || g.is_symb_of_sommet(at_pointplus)) {
      const gen &arg=g._SYMBptr->feuille;
      if (vectarg) {
        ml.priority=_PRIORITY_ADD;
        vecteur args=flatten_operands(g);
        for (const_iterateur it=args.begin();it!=args.end();++it) {
          tmp=gen2markup(*it,flags,idc,contextptr);
          if (!tmp.neg && it!=args.begin()) {
            if (mml_presentation)
              ml.markup+=(g.is_symb_of_sommet(at_plus)?mml_plus:"<mo>&oplus;</mo>");
            if (tex)
              ml.latex+=(g.is_symb_of_sommet(at_plus)?"+":"\\oplus ");
          }
          prepend_minus(tmp,flags,g.is_symb_of_sommet(at_pointplus),false);
          if (tmp.priority>=ml.priority)
            parenthesize(tmp,flags);
          if (mml_content)
            ml.content+=tmp.content;
          if (mml_presentation)
            ml.markup+=tmp.markup;
          if (tex)
            ml.latex+=tmp.latex;
        }
        if (mml_content)
          ml.content=mml_tag(
              "apply",(g.is_symb_of_sommet(at_plus)?"<plus/>":"<ci>.+</ci>")+ml.content,++idc);
        if (mml_presentation)
          ml.markup=mml_tag("mrow",ml.markup,idc);
        return ml;
      } else if (g.is_symb_of_sommet(at_plus)) {
        ml=gen2markup(arg,flags,idc,contextptr);
        if (!ml.neg && ml.priority==0) {
          string id;
          if (mml_content) {
            id=extract_id(ml,true);
            ml.content=mml_tag("apply","<plus/>"+ml.content,0,"id",id);
          }
          if (mml_presentation) {
            id=extract_id(ml,false);
            ml.markup=mml_tag("mrow",mml_plus+ml.markup,0,"xref",id);
          }
          if (tex)
            ml.latex="+"+ml.latex;
          ml.priority=_PRIORITY_ADD;
        }
        return ml;
      }
    }
    if (g.is_symb_of_sommet(at_pointminus) && vectarg && isbinary) {
      ml.priority=_PRIORITY_ADD;
      get_leftright(g._SYMBptr->feuille,NULL,left,right,flags,idc,contextptr);
      prepend_minus(left,flags);
      if (left.priority>=ml.priority)
        parenthesize(left,flags);
      if (mml_content)
        ml.content=mml_tag("apply","<ci>"+string(right.neg?".+":".-")+"</ci>"+
                                      left.content+right.content,
                             ++idc);
      string op=(right.neg?"<mo>&oplus;</mo>":"<mo>&ominus;</mo>");
      string op_tex=(right.neg?"\\oplus ":"\\ominus ");
      if (right.priority>=ml.priority)
        parenthesize(right,flags);
      if (mml_presentation)
        ml.markup=mml_tag("mrow",left.markup+op+right.markup,idc);
      if (tex)
        ml.latex=left.latex+op_tex+right.latex;
      return ml;
    }
    if ((g.is_symb_of_sommet(at_prod) || g.is_symb_of_sommet(at_ampersand_times)) && vectarg) {
      int neg_count=0,num_count=0,den_count=0,nc=0,dc=0;
      MarkupBlock pden,pnum,prev;
      string num,numc,numt,den,denc,dent,prod_sign,prod_sign_tex;
      vecteur args=flatten_operands(g),ncnst,cnsts;
      for (int i=args.size();i-->0;) {
        if (args[i].is_integer() || args[i].type==_FRAC) {
          ncnst.push_back(args[i]);
          args.erase(args.begin()+i);
        } else if (args[i]==cst_pi || args[i]==cst_euler_gamma) {
          cnsts.push_back(args[i]);
          args.erase(args.begin()+i);
        } else if (args[i].type==_CPLX && is_zero(re(args[i],contextptr)) &&
                   !is_one(im(args[i],contextptr))) {
          if (is_minus_one(im(args[i],contextptr)))
            ml.neg=!ml.neg;
          else cnsts.push_back(im(args[i],contextptr));
          args[i]=cst_i;
        }
      }
      args=mergevecteur(mergevecteur(ncnst,cnsts),args);
      ml.priority=_PRIORITY_MUL;
      pden.type=_MLBLOCK_GENERAL;
      pden.appl=false;
      pden.priority=0;
      pnum.type=_MLBLOCK_GENERAL;
      pnum.appl=false;
      pnum.priority=0;
      bool isinv,hasleadingfrac=true,is_cdot;
      int ni,di,np=-1,dp=-1;
      for (iterateur it=args.begin();it!=args.end();++it) {
        if (it->is_symb_of_sommet(at_inv)) {
          den_count++;
          if (!hasleadingfrac)
            continue;
          if (it->_SYMBptr->feuille.type==_INT_ && dp<0)
            dp=it-args.begin();
          else {
            dp=-1;
            hasleadingfrac=false;
          }
        } else {
          num_count++;
          if (hasleadingfrac && np<0 && it->type==_INT_)
            np=it-args.begin();
        }
      }
      if (num_count==1 || dp<0)
        hasleadingfrac=false;
      if (hasleadingfrac) {
        di=args[dp]._SYMBptr->feuille.val;
        args.erase(args.begin()+dp);
        if (np>dp)
          np--;
        if (np<0)
          args.insert(args.begin(),fraction(1,di));
        else {
          ni=args[np].val;
          args.erase(args.begin()+np);
          args.insert(args.begin(),fraction(ni,di));
        }
        num_count=den_count=0;
        for (const_iterateur it=args.begin();it!=args.end();++it) {
          if (it->is_symb_of_sommet(at_inv))
            den_count++;
          else
            num_count++;
        }
      }
      for (const_iterateur it=args.begin();it!=args.end();++it) {
        isinv=it->is_symb_of_sommet(at_inv);
        int flg=flags;
        if ((isinv && den_count>1) || (!isinv && num_count>1))
          flg|=_MARKUP_FACTOR;
        gen fc=isinv?it->_SYMBptr->feuille:*it;
        tmp=gen2markup(fc,flg,idc,contextptr);
        if (tmp.neg)
          neg_count++;
        if (isinv) {
          dc++;
          prev=pden;
        } else {
          nc++;
          prev=pnum;
        }
        if (((!isinv && num_count>1) || (isinv && den_count>1)) &&
            (tmp.priority>ml.priority || (tmp.priority==ml.priority && fc.type!=_CPLX)) &&
            (!tmp.ctype(_MLBLOCK_FRACTION) &&
             (!tmp.ctype(_MLBLOCK_SUMPROD) || (!isinv && nc<num_count) ||
              (isinv && dc<den_count))))
          parenthesize(tmp,flags);
        is_cdot=
            tmp.ctype(_MLBLOCK_NUMERIC_EXACT) || tmp.ctype(_MLBLOCK_NUMERIC_APPROX) ||
            tmp.ctype(_MLBLOCK_SUBTYPE_IDNT) || tmp.ctype(_MLBLOCK_FACTORIAL) ||
            prev.ctype(_MLBLOCK_SUBTYPE_IDNT) ||
            (tmp.ctype(_MLBLOCK_FRACTION) && prev.ctype(_MLBLOCK_FRACTION)) ||
            (prev.ctype(_MLBLOCK_ELEMAPP) && prev.appl);
        prod_sign=is_cdot?mml_cdot:mml_itimes;
        prod_sign_tex=is_cdot?"\\cdot ":tex_implmul(tmp,prev);
        if (isinv) {
          pden=tmp;
          if (mml_content)
            denc+=tmp.content;
          if (mml_presentation)
            den+=(den.empty()?"":prod_sign)+tmp.markup;
          if (tex)
            dent+=(dent.empty()?"":prod_sign_tex)+tmp.latex;
        } else {
          pnum=tmp;
          if (mml_content)
            numc+=tmp.content;
          if (mml_presentation)
            num+=(num.empty()?"":prod_sign)+tmp.markup;
          if (tex)
            numt+=(numt.empty()?"":prod_sign_tex)+tmp.latex;
        }
      }
      if (neg_count % 2) ml.neg=!ml.neg;
      if (mml_content && numc.empty())
        numc="<cn type='integer'>1</cn>";
      if (mml_presentation && num.empty())
        num="<mn>1</mn>";
      if (tex && numt.empty())
        numt="1";
      if (mml_content && denc.empty())
        ml.content=mml_tag("apply","<times/>"+numc,++idc);
      else if (mml_content) {
        ml.type=_MLBLOCK_FRACTION;
        ml.content=mml_tag("apply","<divide/>"+numc+denc,++idc);
      }
      if (mml_presentation && den.empty())
        ml.markup=mml_tag("mrow",num,idc);
      else if (mml_presentation) {
        ml.type=_MLBLOCK_FRACTION;
        ml.markup=mml_tag("mfrac",num+den,idc);
      }
      if (tex && dent.empty())
        ml.latex=numt;
      else if (tex) {
        ml.type=_MLBLOCK_FRACTION;
        ml.latex="\\frac{"+numt+"}{"+dent+"}";
      }
      return ml;
    }
    if (g.is_symb_of_sommet(at_pointprod) && vectarg) {
      ml.priority=_PRIORITY_MUL;
      vecteur args=flatten_operands(g);
      for (const_iterateur it=args.begin();it!=args.end();++it) {
        tmp=gen2markup(*it,flags,idc,contextptr);
        prepend_minus(tmp,flags);
        if (tmp.priority>=ml.priority)
          parenthesize(tmp,flags);
        if (mml_content)
          ml.content+=tmp.content;
        if (mml_presentation)
          ml.markup+=(it==args.begin()?"":"<mo>&odot;</mo>")+tmp.markup;
        if (tex)
          ml.latex+=(it==args.begin()?"":"\\odot ")+tmp.latex;
      }
      if (mml_content)
        ml.content=mml_tag("apply","<ci>.*</ci>"+ml.content,++idc);
      if (mml_presentation)
        ml.markup=mml_tag("mrow",ml.markup,idc);
      return ml;
    }
    if ((g.is_symb_of_sommet(at_dot) || g.is_symb_of_sommet(at_dotP) ||
         g.is_symb_of_sommet(at_scalarProduct) ||
         g.is_symb_of_sommet(at_scalar_product) || g.is_symb_of_sommet(at_dotprod) ||
         g.is_symb_of_sommet(at_cross) ||
         g.is_symb_of_sommet(at_crossP) || g.is_symb_of_sommet(at_crossproduct)) &&
        vectarg && isbinary) {
      ml.priority=_PRIORITY_MUL;
      get_leftright(g._SYMBptr->feuille,&ml,left,right,flags,idc,
                    contextptr);
      bool is_cross=(g.is_symb_of_sommet(at_cross) || g.is_symb_of_sommet(at_crossP) ||
                       g.is_symb_of_sommet(at_crossproduct));
      if (mml_content)
        ml.content=mml_tag("apply",(is_cross?"<vectorproduct/>":"<scalarproduct/>")+
                           left.content+right.content,
                           ++idc);
      if (mml_presentation)
        ml.markup=mml_tag(
            "mrow",left.markup+(is_cross?mml_times:mml_dot)+right.markup,idc);
      if (tex)
        ml.latex=left.latex+(is_cross?"\\times ":"\\cdot ")+right.latex;
      return ml;
    }
    if (g.is_symb_of_sommet(at_inv)) {
      tmp=gen2markup(g._SYMBptr->feuille,flags,idc,contextptr);
      ml.neg=tmp.neg;
      ml.type=_MLBLOCK_FRACTION;
      ml.priority=_PRIORITY_MUL;
      if (mml_content)
        ml.content=mml_tag("apply","<divide/><cn type='integer'>1</cn>"+tmp.content,++idc);
      if (mml_presentation)
        ml.markup=mml_tag("mfrac","<mn>1</mn>"+tmp.markup,idc);
      if (tex)
        ml.latex="\\frac1{"+tmp.latex+"}";
      return ml;
    }
    if ((g.is_symb_of_sommet(at_division) || g.is_symb_of_sommet(at_rdiv)) && vectarg && isbinary) {
      const vecteur &args=*g._SYMBptr->feuille._VECTptr;
      MarkupBlock num,den;
      ml.type=_MLBLOCK_FRACTION;
      ml.priority=_PRIORITY_MUL;
      num=gen2markup(args.front(),flags,idc,contextptr);
      den=gen2markup(args.back(),flags,idc,contextptr);
      ml.neg=(num.neg!=den.neg);
      if (mml_content)
        ml.content=mml_tag("apply","<divide/>"+num.content+den.content,++idc);
      if (mml_presentation)
        ml.markup=mml_tag("mfrac",num.markup+den.markup,idc);
      if (tex)
        ml.latex="\\frac{"+num.latex+"}{"+den.latex+"}";
      return ml;
    }
    if (g.is_symb_of_sommet(at_pointdivision) && vectarg && isbinary) {
      ml.priority=_PRIORITY_MUL;
      get_leftright(g._SYMBptr->feuille,&ml,left,right,flags,idc,contextptr);
      if (mml_content)
        ml.content=mml_tag(
            "apply","<ci>./</ci>"+left.content+right.content,++idc);
      if (mml_presentation)
        ml.markup=mml_tag(
            "mrow",left.markup+"<mo>&oslash;</mo>"+right.markup,idc);
      if (tex)
        ml.latex=left.latex+"\\oslash "+right.latex;
      return ml;
    }
    if (g.is_symb_of_sommet(at_conj)) {
      tmp=gen2markup(g._SYMBptr->feuille,flags,idc,contextptr);
      if (isfactor && tmp.priority>_PRIORITY_MUL)
        parenthesize(tmp,flags);
      ml.priority=_PRIORITY_UNARY;
      ml.neg=tmp.neg;
      if (mml_content)
        ml.content=mml_tag("apply","<conjugate/>"+tmp.content,++idc);
      if (mml_presentation)
        ml.markup=mml_tag("mover",tmp.markup+"<mo>&#xaf;</mo>",idc);
      if (tex)
        ml.latex="\\overline{"+tmp.latex+"}";
      return ml;
    }
    if (g.is_symb_of_sommet(at_exp) || g.is_symb_of_sommet(at_EXP)) {
      if (is_one(g._SYMBptr->feuille)) {
        ml.priority=0;
        if (mml_content)
          ml.content=insert_id("<exponentiale/>",++idc,true);
        if (mml_presentation)
          ml.markup=insert_id(mml_e,idc,false);
        if (tex)
          ml.latex="\\mathrm{e}";
      } else {
        tmp=gen2markup(g._SYMBptr->feuille,flags,idc,contextptr);
        prepend_minus(tmp,flags);
        ml.priority=_PRIORITY_EXP;
        ml.type=_MLBLOCK_POWER;
        if (mml_content)
          ml.content=mml_tag("apply","<exp/>"+tmp.content,++idc);
        if (mml_presentation)
          ml.markup=mml_tag("msup",mml_e+tmp.markup,idc);
        if (tex)
          ml.latex="\\mathrm{e}^{"+tmp.latex+"}";
      }
      return ml;
    }
    if (g.is_symb_of_sommet(at_sq)) {
      ml.priority=_PRIORITY_EXP;
      ml.type=_MLBLOCK_POWER;
      tmp=gen2markup(g._SYMBptr->feuille,flags,idc,contextptr);
      if (tmp.priority>=ml.priority)
        parenthesize(tmp,flags);
      if (mml_content)
        ml.content=mml_tag("apply","<power/>"+tmp.content+"<cn type='integer'>2</cn>",++idc);
      if (mml_presentation)
        ml.markup=mml_tag("msup",tmp.markup+"<mn>2</mn>",idc);
      if (tex)
        ml.latex=tmp.latex+"^{2}";
      return ml;
    }
    if (g.is_symb_of_sommet(at_sqrt)) {
      ml.priority=_PRIORITY_UNARY;
      ml.type=_MLBLOCK_ROOT;
      tmp=gen2markup(g._SYMBptr->feuille,flags,idc,contextptr);
      prepend_minus(tmp,flags);
      if (mml_content)
        ml.content=mml_tag("apply","<root/>"+tmp.content,++idc);
      if (mml_presentation)
        ml.markup=mml_tag("msqrt",tmp.markup,idc);
      if (tex)
        ml.latex="\\sqrt{"+tmp.latex+"}";
      return ml;
    }
    if (g.is_symb_of_sommet(at_maple_root) && vectarg) {
      ml.priority=_PRIORITY_UNARY;
      ml.type=_MLBLOCK_ROOT;
      const vecteur &args=*g._SYMBptr->feuille._VECTptr;
      right=gen2markup(args.front(),flags,idc,contextptr);
      prepend_minus(right,flags);
      left=gen2markup(args.back(),flags,idc,contextptr);
      prepend_minus(left,flags);
      if (mml_content)
        ml.content=mml_tag("apply","<root/><degree>"+right.content+"</degree>"+left.content,++idc);
      if (mml_presentation)
        ml.markup=mml_tag("mroot",left.markup+right.markup,idc);
      if (tex)
        ml.latex="\\sqrt["+right.latex+"]{"+left.latex+"}";
      return ml;
    }
    if ((g.is_symb_of_sommet(at_pow) || g.is_symb_of_sommet(at_matpow) ||
        g.is_symb_of_sommet(at_pointpow)) && vectarg && isbinary) {
      ml.priority=_PRIORITY_EXP;
      const vecteur &args=*g._SYMBptr->feuille._VECTptr;
      left=gen2markup(args.front(),flags,idc,contextptr);
      prepend_minus(left,flags);
      if (g.is_symb_of_sommet(at_pow) && is_one(2*args.back())) {
        ml.type=_MLBLOCK_ROOT;
        ml.priority=_PRIORITY_UNARY;
        if (mml_content)
          ml.content=mml_tag("apply","<root/>"+left.content,++idc);
        if (mml_presentation)
          ml.markup=mml_tag("msqrt",left.markup,idc);
        if (tex)
          ml.latex="\\sqrt{"+left.latex+"}";
      } else {
        right=gen2markup(args.back(),flags,idc,contextptr);
        prepend_minus(right,flags);
        if (mml_content)
          ml.content=mml_tag("apply",(g.is_symb_of_sommet(at_pointpow)?"<ci>.^</ci>":"<power/>")+
                                        left.content+right.content,
                               ++idc);
        if ((flags & _MARKUP_ELEMPOW)!=0 && g.is_symb_of_sommet(at_pow) &&
            right.priority==0 && !is_one(_abs(args.back(),contextptr)) &&
            left.ctype(_MLBLOCK_ELEMAPP) && left.split_pos>=0) {
          ml.type=_MLBLOCK_ELEMAPP;
          ml.appl=left.appl;
          if (mml_presentation) {
            size_t pos=left.markup.find(">");
            assert(pos!=string::npos);
            ml.markup=left.markup.substr(0,pos+1)+"<msup>"+left.markup.substr(pos+1,left.split_pos)+
                      right.markup+"</msup>"+left.markup.substr(left.split_pos+pos+1);
          }
          if (tex)
            ml.latex=left.latex.substr(0,left.split_pos_tex)+"^{"+right.latex+"}"+
                     left.latex.substr(left.split_pos_tex);
        } else {
          ml.type=_MLBLOCK_POWER;
          if (left.appl || left.priority>=ml.priority)
            parenthesize(left,flags);
          if (mml_presentation)
            ml.markup=mml_tag("msup",left.markup+right.markup,idc);
          if (tex)
            ml.latex=left.latex+"^{"+right.latex+"}";
        }
      }
      return ml;
    }
    if (g.is_symb_of_sommet(at_abs) || g.is_symb_of_sommet(at_floor) || g.is_symb_of_sommet(at_ceil)) {
      tmp=gen2markup(g._SYMBptr->feuille,flags,idc,contextptr);
      if (!g.is_symb_of_sommet(at_abs))
        prepend_minus(tmp,flags);
      ld=(g.is_symb_of_sommet(at_abs)?"|":(g.is_symb_of_sommet(at_floor)?"&lfloor;":"&lceil;"));
      rd=(g.is_symb_of_sommet(at_abs)?"|":(g.is_symb_of_sommet(at_floor)?"&rfloor;":"&rceil;"));
      ld_tex=(g.is_symb_of_sommet(at_abs)?"|":(g.is_symb_of_sommet(at_floor)?"\\lfloor ":"\\lceil "));
      rd_tex=(g.is_symb_of_sommet(at_abs)?"|":(g.is_symb_of_sommet(at_floor)?"\\rfloor ":"\\rceil "));
      if (mml_content)
        ml.content=mml_tag(
            "apply",(g.is_symb_of_sommet(at_abs)?"<abs/>" :
                         (g.is_symb_of_sommet(at_floor)?"<floor/>":"<ceiling/>"))+tmp.content,++idc);
      if (mml_presentation)
        ml.markup=mml_tag("mfenced",tmp.markup,idc,"open",ld,"close",rd);
      if (tex)
        ml.latex="\\left"+ld_tex+tmp.latex+"\\right"+rd_tex;
      return ml;
    }
    if (g.is_symb_of_sommet(at_laplace) || g.is_symb_of_sommet(at_ilaplace) ||
        g.is_symb_of_sommet(at_invlaplace) || g.is_symb_of_sommet(at_fourier) ||
        g.is_symb_of_sommet(at_ifourier) ||
        g.is_symb_of_sommet(at_ztrans) || g.is_symb_of_sommet(at_invztrans)) {
      const gen &arg=g._SYMBptr->feuille;
      bool has_func=true,has_var1=false,has_var2=false;
      if (arg.type==_VECT && !arg._VECTptr->empty()) {
        const vecteur &args=*arg._VECTptr;
        tmp=gen2markup(args.front(),flags,idc,contextptr);
        if (args.size()>1) {
          left=gen2markup(args[1],flags,idc,contextptr);
          prepend_minus(left,flags);
          has_var1=true;
        }
        if (args.size()>2) {
          right=gen2markup(args[2],flags,idc,contextptr);
          prepend_minus(right,flags);
          has_var2=true;
        }
      } else if (arg.type!=_VECT)
        tmp=gen2markup(arg,flags,idc,contextptr);
      else
        has_func=false;
      if (has_func) {
        ml.neg=tmp.neg;
        ml.priority=_PRIORITY_APPLY;
        str=g._SYMBptr->sommet.ptr()->s;
        string L=(is_substr(str,"laplace")?"L":(is_substr(str,"ztrans")?"Z" :
                          (is_substr(str,"fourier")?"F":"?")));
        if (mml_content)
          ml.content=mml_tag("apply",mml_tag("ci",str)+tmp.content+left.content+right.content,++idc);
        string lap=mml_tag("mi",L,0,"mathvariant","script");
        string lap_tex="\\mathcal{"+L+"}";
        str="<mrow><mo>&minus;</mo><mn>1</mn></mrow>";
        if (g.is_symb_of_sommet(at_ilaplace) || g.is_symb_of_sommet(at_invlaplace) ||
            g.is_symb_of_sommet(at_invztrans) || g.is_symb_of_sommet(at_ifourier)) {
          if (has_var1) {
            lap=mml_tag("msubsup",lap+left.markup+str);
            lap_tex=lap_tex+"_{"+left.latex+"}^{-1}";
          } else {
            lap=mml_tag("msup",lap+str);
            lap_tex=lap_tex+"^{-1}";
          }
        } else if (has_var1) {
          lap=mml_tag("msub",lap+left.markup);
          lap_tex=lap_tex+"_{"+left.latex+"}";
        }
        if (mml_presentation)
          ml.markup=lap+mml_tag("mfenced",tmp.markup,0,"open","{","close","}");
        if (tex)
          ml.latex=lap_tex+"\\left\\{"+tmp.latex+"\\right\\}";
        if (has_var2) {
          parenthesize(right,flags);
          if (mml_presentation)
            ml.markup=ml.markup+right.markup;
          if (tex)
            ml.latex=ml.latex+right.latex;
        }
        if (mml_presentation)
          ml.markup=mml_tag("mrow",ml.markup,idc);
        return ml;
      }
    }
    if (g.is_symb_of_sommet(at_integrate) || g.is_symb_of_sommet(at_int) ||
        g.is_symb_of_sommet(at_Int) || g.is_symb_of_sommet(at_sum) || g.is_symb_of_sommet(at_somme) ||
        g.is_symb_of_sommet(at_add) || g.is_symb_of_sommet(at_product) || g.is_symb_of_sommet(at_mul)) {
      bool has_ub=false,has_lb=false;
      bool isint=(g.is_symb_of_sommet(at_integrate) || g.is_symb_of_sommet(at_int)),
           issum=(g.is_symb_of_sommet(at_sum) || g.is_symb_of_sommet(at_somme) ||
                    g.is_symb_of_sommet(at_add)),
           isprod=(g.is_symb_of_sommet(at_product) || g.is_symb_of_sommet(at_mul));
      string big="<mo>"+string(isint?"&int;":(issum?"&sum;":"&prod;"))+"</mo>";
      string big_tex=(isint?"\\int ":(issum?"\\sum ":"\\prod "));
      MarkupBlock lb,ub,var,e;
      ml.priority=_PRIORITY_MUL;
      ml.type=_MLBLOCK_SUMPROD;
      if (vectarg) {
        const vecteur &args=*g._SYMBptr->feuille._VECTptr;
        e=gen2markup(args[0],flags,idc,contextptr);
        if (args.size()>1) {
          if (args[1].is_symb_of_sommet(at_equal) &&
              args[1]._SYMBptr->feuille._VECTptr->back().is_symb_of_sommet(
                  at_interval)) {
            gen &lh=args[1]._SYMBptr->feuille._VECTptr->front();
            gen &low=args[1]
                           ._SYMBptr->feuille._VECTptr->back()
                           ._SYMBptr->feuille._VECTptr->front();
            gen &up=args[1]
                          ._SYMBptr->feuille._VECTptr->back()
                          ._SYMBptr->feuille._VECTptr->back();
            lb=gen2markup(low,flags,idc,contextptr);
            has_lb=true;
            ub=gen2markup(up,flags,idc,contextptr);
            has_ub=true;
            var=gen2markup(lh,flags,idc,contextptr);
          } else
            var=gen2markup(args[1],flags,idc,contextptr);
          prepend_minus(var,flags);
          if (isint) {
            if (var.priority!=0)
              parenthesize(var,flags);
            if (mml_presentation)
              var.markup=mml_itimes+mml_d+var.markup;
            if (tex)
              var.latex="\\,\\mathrm{d}"+var.latex;
          }
        }
        if (!has_lb && args.size()>2) {
          lb=gen2markup(args[2],flags,idc,contextptr);
          has_lb=true;
        }
        if (!has_ub && args.size()>3) {
          ub=gen2markup(args[3],flags,idc,contextptr);
          has_ub=true;
        }
        if (has_lb)
          prepend_minus(lb,flags);
        if (has_ub)
          prepend_minus(ub,flags);
        if (has_lb && !isint) {
          if (mml_presentation)
            lb.markup=mml_tag("mrow",var.markup+"<mo>=</mo>"+lb.markup);
          if (tex)
            lb.latex=var.latex+"="+lb.latex;
        }
        if (!has_lb && !isint) {
          lb=var;
          has_lb=true;
        }
        if (has_lb && !has_ub) {
          big=mml_tag("munder",big+lb.markup);
          big_tex+="_{"+lb.latex+"}";
        } else if (has_lb && has_ub) {
          big=mml_tag("munderover",big+lb.markup+ub.markup);
          big_tex+="_{"+lb.latex+"}^{"+ub.latex+"}";
        }
      } else
        e=gen2markup(g._SYMBptr->feuille,flags,idc,contextptr);
      if (isprod)
        prepend_minus(e,flags);
      else
        ml.neg=e.neg;
      if ((isprod && !e.ctype(_MLBLOCK_FRACTION) && e.priority>=ml.priority) ||
          (!isprod && (e.ctype(_MLBLOCK_NEG) || e.priority>ml.priority)))
        parenthesize(e,flags);
      if (mml_content) {
        str.clear();
        if (!var.content.empty())
          str+=mml_tag("bvar",var.content);
        if (has_lb && !has_ub)
          str+=mml_tag(isint?"domainofapplication":"condition",lb.content);
        else if (has_lb && has_ub)
          str+=mml_tag("lowlimit",lb.content)+mml_tag("uplimit",ub.content);
        str+=e.content;
        ml.content=mml_tag("apply",(isint?"<int/>":(issum?"<sum/>":"<product/>"))+str,++idc);
      }
      if (mml_presentation)
        ml.markup=mml_tag("mrow",big+e.markup+(isint?var.markup:""),idc);
      if (tex)
        ml.latex=big_tex+e.latex+(isint?var.latex:"");
      return ml;
    }
    if ((g.is_symb_of_sommet(at_limit) || g.is_symb_of_sommet(at_limite)) &&
        vectarg && g._SYMBptr->feuille._VECTptr->size()>1) {
      const vecteur &args=*g._SYMBptr->feuille._VECTptr;
      ml.priority=_PRIORITY_MUL;
      ml.type=_MLBLOCK_SUMPROD;
      MarkupBlock e,var,dest;
      int dir=0;
      e=gen2markup(args[0],flags,idc,contextptr);
      ml.neg=e.neg;
      if (e.priority>ml.priority)
        parenthesize(e,flags);
      else {
        if (mml_presentation)
          e.markup=mml_apply+e.markup;
        if (tex)
          e.latex="\\;"+e.latex;
      }
      if (args.size()>=2) {
        if (args[1].is_symb_of_sommet(at_equal)) {
          var=gen2markup(args[1]._SYMBptr->feuille._VECTptr->front(),flags,idc,contextptr);
          dest=gen2markup(args[1]._SYMBptr->feuille._VECTptr->back(),flags,idc,contextptr);
          if (args.size()==3 && args[2].type==_INT_)
            dir=args[2].val;
        } else if (args.size()>=3) {
          var=gen2markup(args[1],flags,idc,contextptr);
          dest=gen2markup(args[2],flags,idc,contextptr);
          if (args.size()==4 && args[3].type==_INT_)
            dir=args[3].val;
        }
        prepend_minus(var,flags);
        prepend_minus(dest,flags);
        if (mml_content) {
          str.clear();
          if (!var.content.empty()) {
            str+=mml_tag("bvar",var.content);
            if (!dest.content.empty())
              str+=mml_tag("condition",mml_tag("tendsto",var.content+dest.content,0,
                                                  dir==0?"":"type",
                                                  dir<0?"below":"above"));
          }
          str+=e.content;
          ml.content=mml_tag("apply","<limit/>"+str,++idc);
        }
        if (mml_presentation) {
          ml.markup="<munder><mi>lim</mi><mrow>"+var.markup+"<mo>&rarr;</mo>";
          if (dir==0)
            ml.markup+=dest.markup;
          else
            ml.markup+="<msup>"+dest.markup+string(dir<0?mml_minus:mml_plus)+"</msup>";
          ml.markup+="</mrow></munder>"+e.markup;
          ml.markup=mml_tag("mrow",ml.markup,idc);
        }
        if (tex)
          ml.latex="\\lim_{"+var.latex+"\\to "+dest.latex+
                   string(dir==0?"":(dir<0?"^{-}":"^{+}"))+"}"+e.latex;
        return ml;
      }
    }
    if (g.is_symb_of_sommet(at_sum_riemann) && vectarg && isbinary) {
      const vecteur &args=*g._SYMBptr->feuille._VECTptr;
      if (args.back().type==_VECT && args.back()._VECTptr->size()==2) {
        const vecteur &vars=*args.back()._VECTptr;
        ml=gen2markup(symbolic(at_limit,makevecteur(symbolic(at_sum,
                                 makevecteur(args.front(),vars.back(),1,vars.front())),
                                 vars.front(),unsigned_inf)),
                        flags,idc,contextptr);
        return ml;
      }
    }
    if (g.is_symb_of_sommet(at_derive) || g.is_symb_of_sommet(at_deriver)) {
      identificateur f;
      vecteur deg;
      if (is_partialdiff(g,f,deg,contextptr)) {
        ml.priority=_PRIORITY_EXP;
        ml.type=_MLBLOCK_POWER | _MLBLOCK_DERIVATIVE;
        left=gen2markup(f,flags,idc,contextptr);
        right=gen2markup(gen(deg,_SEQ__VECT),flags,idc,contextptr);
        if (tex)
          ml.latex=left.latex+"^{"+right.latex+"}";
        if (mml_content)
          ml.content=mml_tag("apply","<partialdiff/>"+right.content+left.content,++idc);
        if (mml_presentation)
          ml.markup=mml_tag("msup",left.markup+right.markup,idc);
        return ml;
      }
      gen arg=g._SYMBptr->feuille;
      int n=0,cnt=0;
      while (arg.is_symb_of_sommet(at_derive) || arg.is_symb_of_sommet(at_deriver)) {
        arg=arg._SYMBptr->feuille;
        ++n;
      }
      if (n==0 || !vectarg) {
        ml.type=_MLBLOCK_DERIVATIVE;
        if (vectarg)
          tmp=gen2markup(arg._VECTptr->front(),flags,idc,contextptr);
        else
          tmp=gen2markup(arg,flags,idc,contextptr);
        bool simp=(tmp.priority==0);
        if (tmp.priority>_PRIORITY_APPLY)
          parenthesize(tmp,flags);
        ml.neg=tmp.neg;
        if (n>0) {
          ml.priority=_PRIORITY_EXP;
          ml.type|=_MLBLOCK_POWER;
          ++n;
          string N=gen(n).print(contextptr);
          if (mml_content)
            ml.content=mml_tag("apply",mml_csymbol("nthdiff","calculus1")+"<cn>"+N+"</cn>"+tmp.content,
                               ++idc);
          switch (n) {
          case 2:
            if (mml_presentation)
              ml.markup=mml_tag("msup",tmp.markup+"<mo>&Prime;</mo>",idc);
            if (tex)
              ml.latex=tmp.latex+"''";
            break;
          case 3:
            if (mml_presentation)
              ml.markup=mml_tag("msup",tmp.markup+"<mo>&tprime;</mo>",idc);
            if (tex)
              ml.latex=tmp.latex+"'''";
            break;
          default:
            if (mml_presentation)
              ml.markup=mml_tag("msup",tmp.markup+mml_tag("mfenced","<mn>"+N+"</mn>"),idc);
            if (tex)
              ml.latex=tmp.latex+"^{("+N+")}";
            break;
          }
          return ml;
        }
        if (n==0 && !vectarg) {
          ml.priority=_PRIORITY_EXP;
          if (mml_content)
            ml.content=mml_tag("apply","<diff/>"+tmp.content,++idc);
          if (mml_presentation)
            ml.markup=mml_tag("msup",tmp.markup+"<mo>&prime;</mo>",idc);
          if (tex)
            ml.latex=tmp.latex+"'";
          return ml;
        }
        if (vectarg && arg._VECTptr->size()>1) {
          vecteur vars;
          MarkupBlock var;
          if (get_derive_vars(*arg._VECTptr,vars)) {
            int n=vars.size();
            ml.priority=_PRIORITY_MUL;
            ml.type|=_MLBLOCK_FRACTION;
            string N=gen(n).print(contextptr);
            if (n==1 && is_one(vars.front()._VECTptr->back())) {
              var=gen2markup(vars.front()._VECTptr->front(),flags,idc,contextptr);
              if (mml_content)
                ml.content=mml_tag("apply","<diff/><bvar>"+var.content+"</bvar>"+tmp.content,++idc);
              if (mml_presentation)
                ml.markup=simp?mml_tag("mfrac","<mrow>"+mml_d+tmp.markup+
                                               "</mrow><mrow>"+mml_d+var.markup+"</mrow>",idc)
                              :mml_tag("mrow",mml_tag("mfrac",mml_d+mml_tag("mrow",var.markup))+
                                              mml_apply+tmp.markup,idc);
              if (tex)
                ml.latex=simp?"\\frac{\\mathrm{d}"+tmp.latex+"}{\\mathrm{d}"+var.latex+"}"
                             :"\\frac{\\mathrm{d}}{\\mathrm{d}"+var.latex+"}"+tex_itimes+tmp.latex;
              return ml;
            }
            if (vars.size()>0) {
              string ds(vars.size()>1?"<mo>&part;</mo>":mml_d);
              string ds_tex(vars.size()>1?"\\partial ":"\\mathrm{d}");
              int c=0;
              gen N(0);
              for (const_iterateur it=vars.begin();it!=vars.end();++it) {
                var=gen2markup(it->_VECTptr->front(),flags,idc,contextptr);
                str=it->_VECTptr->back().print(contextptr);
                N+=it->_VECTptr->back();
                if (mml_content)
                  ml.content+=mml_tag(
                    "bvar",var.content+(is_one(it->_VECTptr->back())?
                      "":"<degree><cn>"+str+"</cn></degree>"),idc);
                if (mml_presentation)
                  ml.markup+=is_one(it->_VECTptr->back())?ds+var.markup
                                                         :mml_tag("msup","<mrow>"+ds+var.markup+
                                                                         "</mrow><mn>"+str+"</mn>");
                if (tex)
                  ml.latex+=ds_tex+var.latex+(is_one(it->_VECTptr->back())?"":"^{"+str+"}");
                if (c++<cnt) {
                  if (mml_presentation)
                    ml.markup+=mml_itimes;
                  if (tex)
                    ml.latex+=tex_itimes;
                }
              }
              string Ns=N.print(contextptr);
              if (mml_content)
                ml.content=mml_tag("apply",(vars.size()>1?"<partialdiff/>":"<diff/>")+
                                            ml.content+"<degree><cn type='integer'>"+Ns+
                                            "</cn></degree>"+tmp.content,
                                   ++idc);
              if (mml_presentation)
                ml.markup=simp?mml_tag("mfrac","<mrow><msup>"+ds+"<mn>"+Ns+"</mn></msup>"+
                                       tmp.markup+"</mrow><mrow>"+ml.markup+"</mrow>",idc)
                              :mml_tag("mrow",mml_tag("mfrac","<msup>"+ds+"<mn>"+Ns+"</mn></msup>"+
                                              "<mrow>"+ml.markup+"</mrow>")+mml_apply+tmp.markup,idc);
              if (tex)
                ml.latex=simp?"\\frac{"+ds_tex+"^{"+Ns+"}"+tmp.latex+"}{"+ml.latex+"}"
                             :"\\frac{"+ds_tex+"^{"+Ns+"}}{"+ml.latex+"}"+tex_itimes+tmp.latex;
              return ml;
            }
          }
        }
      }
    }
    if ((g.is_symb_of_sommet(at_same) || g.is_symb_of_sommet(at_different) ||
         g.is_symb_of_sommet(at_sto) || g.is_symb_of_sommet(at_array_sto) ||
         g.is_symb_of_sommet(at_equal)) && isbinary) {
      ml.priority=(g.is_symb_of_sommet(at_sto) || g.is_symb_of_sommet(at_array_sto)
                        ?_PRIORITY_ASSGN
                        :(g.is_symb_of_sommet(at_equal)?_PRIORITY_EQ:_PRIORITY_COMP));
      if (g.is_symb_of_sommet(at_sto) || g.is_symb_of_sommet(at_array_sto))
        get_leftright(g._SYMBptr->feuille,&ml,right,left,flags,idc,contextptr);
      else
        get_leftright(g._SYMBptr->feuille,&ml,left,right,flags,idc,contextptr);
      string op(g.is_symb_of_sommet(at_same)
                   ?"&#x2a75;"
                   :(g.is_symb_of_sommet(at_different)
                          ?"&#x2260;"
                          :(g.is_symb_of_sommet(at_equal)?"=":(g.is_symb_of_sommet(at_sto)?"&#x2254;":"&larr;"))));
      string op_tex(
          g.is_symb_of_sommet(at_same)
             ?"==":(g.is_symb_of_sommet(at_different)?"\\neq "
                                         :(g.is_symb_of_sommet(at_equal)?"="
                                                          :(g.is_symb_of_sommet(at_sto)?":=":"\\leftarrow"))));
      string csymb(
          g.is_symb_of_sommet(at_same)
             ?"==":(g.is_symb_of_sommet(at_different)?"<neq/>"
                                         :(g.is_symb_of_sommet(at_equal)?"<eq/>"
                                                          :(g.is_symb_of_sommet(at_sto)?":=":"=&lt;"))));
      if (csymb[0]!='<')
        csymb=mml_tag("ci",csymb);
      if (mml_content)
        ml.content=mml_tag("apply",csymb+left.content+right.content,++idc);
      if (mml_presentation)
        ml.markup=mml_tag("mrow",left.markup+"<mo>"+op+"</mo>"+right.markup,idc);
      if (tex)
        ml.latex=left.latex+op_tex+right.latex;
      return ml;
    }
    if ((g.is_symb_of_sommet(at_inferieur_strict) || g.is_symb_of_sommet(at_inferieur_egal) ||
         g.is_symb_of_sommet(at_superieur_strict) || g.is_symb_of_sommet(at_superieur_egal)) &&
        vectarg && isbinary) {
      ml.priority=_PRIORITY_INEQ;
      get_leftright(g._SYMBptr->feuille,&ml,left,right,flags,idc,contextptr);
      string op,op_tex,csymb;
      if (g.is_symb_of_sommet(at_inferieur_strict)) {
        op="&lt;";
        op_tex="<";
        csymb="<lt/>";
      } else if (g.is_symb_of_sommet(at_inferieur_egal)) {
        op="&leq;";
        op_tex="\\leq ";
        csymb="<leq/>";
      } else if (g.is_symb_of_sommet(at_superieur_strict)) {
        op="&gt;";
        op_tex=">";
        csymb="<gt/>";
      } else if (g.is_symb_of_sommet(at_superieur_egal)) {
        op="&geq;";
        op_tex="\\geq ";
        csymb="<geq/>";
      }
      if (mml_content)
        ml.content=mml_tag("apply",csymb+left.content+right.content,++idc);
      if (mml_presentation)
        ml.markup=mml_tag("mrow",left.markup+"<mo>"+op+"</mo>"+right.markup,idc);
      if (tex)
        ml.latex=left.latex+op_tex+right.latex;
      return ml;
    }
    if (g.is_symb_of_sommet(at_tilocal) && vectarg && g._SYMBptr->feuille._VECTptr->size()>1) {
      const vecteur &args=*g._SYMBptr->feuille._VECTptr;
      ml.priority=_PRIORITY_OTHER;
      tmp=gen2markup(args.front(),flags,idc,contextptr);
      ml.neg=tmp.neg;
      MarkupBlock sub;
      for (const_iterateur it=args.begin()+1;it!=args.end();++it) {
        sub=gen2markup(*it,flags | _MARKUP_TOPLEVEL,idc,contextptr);
        prepend_minus(sub,flags);
        if (mml_content)
          ml.content+=sub.content;
        if (mml_presentation)
          ml.markup+=sub.markup;
        if (tex)
          ml.latex+=(it==args.begin()+1?"":",")+sub.latex;
      }
      if (mml_content)
        ml.content=mml_tag("apply","<ci>subst</ci>"+ml.content,++idc);
      if (mml_presentation)
        ml.markup=mml_tag(
            "msub",mml_tag("mfenced",tmp.markup,0,"open","","close","|")+
                    mml_tag("mfenced",ml.markup,0,"open","","close",""),
            idc);
      if (tex)
        ml.latex="\\left."+tmp.latex+"\\right|_{"+ml.latex+"}";
      return ml;
    }
    if ((g.is_symb_of_sommet(at_union) || g.is_symb_of_sommet(at_intersect)) && vectarg) {
      ml.priority=_PRIORITY_SET;
      string op=(g.is_symb_of_sommet(at_union)?"&cup;":"&cap;");
      string opc=(g.is_symb_of_sommet(at_union)?"<union/>":"<intersect/>");
      string opt=(g.is_symb_of_sommet(at_union)?"\\cup ":"\\cap ");
      assoc2markup(flatten_operands(g),ml,op,opc,opt,flags,idc,
                   contextptr);
      return ml;
    }
    if (g.is_symb_of_sommet(at_minus) && vectarg && isbinary) {
      ml.priority=_PRIORITY_SET;
      get_leftright(g._SYMBptr->feuille,&ml,left,right,flags,idc,contextptr);
      if (mml_content)
        ml.content=mml_tag(
            "apply","<setdiff/>"+left.content+right.content,++idc);
      if (mml_presentation)
        ml.markup=mml_tag(
            "mrow",left.markup+"<mo>&setminus;</mo>"+right.markup,idc);
      if (tex)
        ml.latex=left.latex+"\\setminus "+right.latex;
      return ml;
    }
    if (g.is_symb_of_sommet(at_member) && vectarg && isbinary &&
        is_set_or_ident(g._SYMBptr->feuille._VECTptr->back())) {
      ml.priority=_PRIORITY_INEQ;
      get_leftright(g._SYMBptr->feuille,&ml,left,right,flags,idc,contextptr);
      if (mml_content)
        ml.content=mml_tag("apply","<in/>"+left.content+right.content,++idc);
      if (mml_presentation)
        ml.markup=mml_tag("mrow",left.markup+"<mo>&isin;</mo>"+right.markup,idc);
      if (tex)
        ml.latex=left.latex+"\\in "+right.latex;
      return ml;
    }
    if (g.is_symb_of_sommet(at_is_included) && vectarg && isbinary &&
        is_set_or_ident(g._SYMBptr->feuille._VECTptr->front()) &&
        is_set_or_ident(g._SYMBptr->feuille._VECTptr->back())) {
      ml.priority=_PRIORITY_INEQ;
      get_leftright(g._SYMBptr->feuille,&ml,left,right,flags,idc,contextptr);
      if (mml_content)
        ml.content=mml_tag("apply","<subset/>"+left.content+right.content,++idc);
      if (mml_presentation)
        ml.markup=mml_tag("mrow",left.markup+"<mo>&sube;</mo>"+right.markup,idc);
      if (tex)
        ml.latex=left.latex+"\\subseteq "+right.latex;
      return ml;
    }
    if (g.is_symb_of_sommet(at_not)) {
      ml.priority=_PRIORITY_NOT;
      tmp=gen2markup(g._SYMBptr->feuille,flags,idc,contextptr);
      prepend_minus(tmp,flags);
      if (tmp.priority>=ml.priority)
        parenthesize(tmp,flags);
      if (mml_content)
        ml.content=mml_tag("apply","<not/>"+tmp.content,++idc);
      if (mml_presentation)
        ml.markup=mml_tag("mrow","<mo>&not;</mo>"+tmp.markup,idc);
      if (tex)
        ml.latex="\\neg "+tmp.latex;
      return ml;
    }
    if ((g.is_symb_of_sommet(at_and) || g.is_symb_of_sommet(at_et) || g.is_symb_of_sommet(at_ou) ||
         g.is_symb_of_sommet(at_xor)) && vectarg) {
      ml.priority=(g.is_symb_of_sommet(at_xor)?_PRIORITY_XOR:(g.is_symb_of_sommet(at_ou)?_PRIORITY_OR:_PRIORITY_AND));
      string op=(g.is_symb_of_sommet(at_xor)?"&veebar;":(g.is_symb_of_sommet(at_ou)?"&or;":"&and;"));
      string opc=(g.is_symb_of_sommet(at_xor)?"<xor/>":(g.is_symb_of_sommet(at_ou)?"<or/>":"<and/>"));
      string opt=(g.is_symb_of_sommet(at_xor)?"\\veebar ":(g.is_symb_of_sommet(at_ou)?"\\vee ":"\\wedge "));
      assoc2markup(flatten_operands(g),ml,op,opc,opt,flags,idc,contextptr);
      return ml;
    }
    if (g.is_symb_of_sommet(at_interval) && vectarg && isbinary) {
      ml.priority=_PRIORITY_SET;
      get_leftright(g._SYMBptr->feuille,&ml,left,right,flags,idc,contextptr);
      if (mml_content)
        ml.content=mml_tag("interval",left.content+right.content,++idc);
      if (mml_presentation)
        ml.markup=mml_tag("mrow",left.markup+"<mo>&#x2025;</mo>"+right.markup,idc);
      if (tex)
        ml.latex=left.latex+"\\mathbin{{.}{.}}"+right.latex;
      return ml;
    }
    if (g.is_symb_of_sommet(at_dollar) && vectarg && isbinary) {
      const vecteur &args=*g._SYMBptr->feuille._VECTptr;
      left=gen2markup(args.front(),flags,idc,contextptr);
      prepend_minus(left,flags);
      parenthesize(left,flags);
      right=gen2markup(args.back(),flags,idc,contextptr);
      prepend_minus(right,flags);
      ml.priority=_PRIORITY_EXP;
      if (mml_content)
        ml.content=mml_tag("apply","<ci>$</ci>"+left.content+right.content,++idc);
      if (mml_presentation)
        ml.markup=mml_tag("msub",left.markup+right.markup,idc);
      if (tex)
        ml.latex=left.latex+"_{"+right.latex+"}";
      return ml;
    }
    if (g.is_symb_of_sommet(at_re) || g.is_symb_of_sommet(at_im)) {
      tmp=gen2markup(g._SYMBptr->feuille,flags,idc,contextptr);
      parenthesize(tmp,flags);
      ml.neg=tmp.neg;
      ml.priority=_PRIORITY_APPLY;
      if (mml_content)
        ml.content=mml_tag("apply",(g.is_symb_of_sommet(at_re)?"<real/>":"<imaginary/>")+tmp.content,++idc);
      if (mml_presentation)
        ml.markup=mml_tag("mrow",(g.is_symb_of_sommet(at_re)?mml_re:mml_im)+tmp.markup,idc);
      if (tex)
        ml.latex=(g.is_symb_of_sommet(at_re)?"\\Re ":"\\Im ")+tmp.latex;
      return ml;
    }
    if (g.is_symb_of_sommet(at_besselJ) || g.is_symb_of_sommet(at_BesselJ) ||
        g.is_symb_of_sommet(at_besselY) || g.is_symb_of_sommet(at_BesselY)) {
      ml.priority=_PRIORITY_APPLY;
      int k=(g.is_symb_of_sommet(at_besselJ) || g.is_symb_of_sommet(at_BesselJ)?1:2);
      str=(k==1?"J":"Y");
      if (g.is_symb_of_sommet(at_BesselJ) || g.is_symb_of_sommet(at_BesselY))
        get_leftright(g._SYMBptr->feuille,NULL,left,right,flags,idc,contextptr);
      else
        get_leftright(g._SYMBptr->feuille,NULL,right,left,flags,idc,contextptr);
      parenthesize(right,flags);
      if (mml_content)
        ml.content=mml_tag("apply",mml_csymbol(k==1?"besselJ":"besselY","hypergeo2")+
                                   left.content+right.content,++idc);
      if (mml_presentation)
        ml.markup=mml_tag("mrow","<msub><mi>"+str+"</mi>"+left.markup+"</msub>"+
                                 mml_apply+right.markup,idc);
      if (tex)
        ml.latex=str+"_{"+left.latex+"}"+right.latex;
      return ml;
    }
    if (g.is_symb_of_sommet(at_grad) || g.is_symb_of_sommet(at_curl) ||
        g.is_symb_of_sommet(at_divergence) ||
        (g.is_symb_of_sommet(at_laplacian) && _evalf(g._SYMBptr->feuille,contextptr).type!=_DOUBLE_)) {
      ml.priority=(g.is_symb_of_sommet(at_grad) || g.is_symb_of_sommet(at_laplacian)?_PRIORITY_APPLY:_PRIORITY_MUL);
      string op=(g.is_symb_of_sommet(at_laplacian)?"<mi>&Delta;</mi>":"<mi>&nabla;</mi>");
      string opc=(g.is_symb_of_sommet(at_grad)?"<grad/>"
                                :(g.is_symb_of_sommet(at_curl)?"<curl/>"
                                                :(g.is_symb_of_sommet(at_divergence)?"<divergence/>"
                                                                                    :"<laplacian/>")));
      string opt=(g.is_symb_of_sommet(at_laplacian)?"\\Delta ":"\\nabla ");
      const gen &arg=g._SYMBptr->feuille;
      if (arg.type!=_VECT) {
        tmp=gen2markup(arg,flags,idc,contextptr);
        ml.neg=tmp.neg;
        if ((ml.priority==_PRIORITY_APPLY && tmp.priority>ml.priority) ||
            (ml.priority==_PRIORITY_MUL && tmp.priority>=ml.priority))
          parenthesize(tmp,flags);
        if (mml_content)
          ml.content=mml_tag("apply",opc+tmp.content,++idc);
        if (g.is_symb_of_sommet(at_grad) || g.is_symb_of_sommet(at_laplacian)) opc+=mml_apply;
        if (g.is_symb_of_sommet(at_divergence)) { opc+=mml_cdot;opt+="\\cdot ";}
        if (g.is_symb_of_sommet(at_curl)) { opc+=mml_times;opt+="\\times ";}
        if (mml_presentation)
          ml.markup=mml_tag("mrow",opc+tmp.markup,idc);
        if (tex)
          ml.latex=opt+tmp.latex;
        return ml;
      }
      if (vectarg && isbinary) {
        const vecteur &args=*arg._VECTptr;
        tmp=gen2markup(args.front(),flags,idc,contextptr);
        if ((ml.priority==_PRIORITY_APPLY && tmp.priority>ml.priority) ||
            (ml.priority==_PRIORITY_MUL && tmp.priority>=ml.priority))
          parenthesize(tmp,flags);
        if (args.back().type!=_VECT) {
          right=gen2markup(args.back(),flags,idc,contextptr);
          if (mml_content)
            ml.content=mml_tag("apply",opc+mml_tag("bvar",right.content)+tmp.content,++idc);
          if (mml_presentation)
            ml.markup=mml_tag(
                "mrow",mml_tag("msub",op+right.markup)+(g.is_symb_of_sommet(at_divergence)?
                       mml_cdot:(g.is_symb_of_sommet(at_curl)?mml_times:""))+tmp.markup,idc);
          if (tex)
            ml.latex=opt+"_{"+right.latex+"}"+(g.is_symb_of_sommet(at_divergence)?
                     "\\cdot ":(g.is_symb_of_sommet(at_curl)?"\\times ":""))+tmp.latex;
        } else {
          MarkupBlock var;
          const vecteur &vars=*args.back()._VECTptr;
          for (const_iterateur it=vars.begin();it!=vars.end();++it) {
            var=gen2markup(*it,flags,idc,contextptr);
            prepend_minus(var,flags);
            if (mml_content)
              ml.content+=mml_tag("bvar",var.content);
            if (mml_presentation)
              ml.markup+=(it!=vars.begin()?mml_icomma:"")+var.markup;
            if (tex)
              ml.latex+=(it!=vars.begin()?" ":"")+var.latex;
          }
          if (mml_content)
            ml.content=mml_tag("apply",opc+ml.content+tmp.content,++idc);
          if (mml_presentation) {
            ml.markup=mml_tag("mrow",ml.markup);
            ml.markup=mml_tag(
                "mrow",mml_tag("msub",op+ml.markup)+(g.is_symb_of_sommet(at_divergence)?
                       mml_cdot:(g.is_symb_of_sommet(at_curl)?mml_times:""))+tmp.markup,idc);
          }
          if (tex)
            ml.latex=opt+"_{"+ml.latex+"}"+(g.is_symb_of_sommet(at_divergence)?
                     "\\cdot ":(g.is_symb_of_sommet(at_curl)?"\\times ":""))+tmp.latex;
        }
        return ml;
      }
    }
    if ((g.is_symb_of_sommet(at_mean) || g.is_symb_of_sommet(at_moyenne) ||
         g.is_symb_of_sommet(at_stddev) || g.is_symb_of_sommet(at_ecart_type) ||
         g.is_symb_of_sommet(at_variance) || g.is_symb_of_sommet(at_median)) &&
         (!vectarg || (g._SYMBptr->feuille.subtype!=_SEQ__VECT &&
                       !ckmatrix(g._SYMBptr->feuille)))) {
      ml.priority=_PRIORITY_APPLY;
      int f=(g.is_symb_of_sommet(at_variance)?3:(g.is_symb_of_sommet(at_median)?4 :
              (g.is_symb_of_sommet(at_mean) || g.is_symb_of_sommet(at_moyenne)?1:2)));
      string op=(f==1?"Mean":(f==2?"SD":(f==3?"Var":"Med")));
      string opc=(f==1?"<mean/>":(f==2?"<sdev/>":(f==3?"<variance/>":"<median/>")));
      string opt="\\operatorname{\\mathrm{"+op+"}}";
      op=mml_tag("mi",op)+mml_apply;
      if (vectarg) {
        const vecteur &args=*g._SYMBptr->feuille._VECTptr;
        for (const_iterateur it=args.begin();it!=args.end();++it) {
          tmp=gen2markup(*it,flags,idc,contextptr);
          prepend_minus(tmp,flags);
          if (mml_content)
            ml.content+=tmp.content;
          if (mml_presentation)
            ml.markup+=tmp.markup;
          if (tex)
            ml.latex+=(it!=args.begin()?",":"")+tmp.latex;
        }
        if (mml_content)
          ml.content=mml_tag("apply",opc+ml.content,++idc);
        if (mml_presentation)
          ml.markup=mml_tag("mrow",op+mml_tag("mfenced",ml.markup,0,"open","[","close","]"),idc);
        if (tex)
          ml.latex=opt+"\\left["+ml.latex+"\\right]";
      } else {
        tmp=gen2markup(g._SYMBptr->feuille,flags,idc,contextptr);
        prepend_minus(tmp,flags);
        if (mml_content)
          ml.content=mml_tag("apply",opc+tmp.content,++idc);
        if (mml_presentation)
          ml.markup=mml_tag("mrow",op+mml_tag("mfenced",tmp.markup,0,"open","[","close","]"),idc);
        if (tex)
          ml.latex=opt+"\\left["+tmp.latex+"\\right]";
      }
      return ml;
    }
    if (g.is_symb_of_sommet(at_normalmod) && vectarg && isbinary) {
      ml.priority=_PRIORITY_MUL;
      get_leftright(g._SYMBptr->feuille,&ml,left,right,flags,idc,contextptr);
      if (mml_content)
        ml.content=mml_tag("apply","<ci>%</ci>"+left.content+right.content,++idc);
      if (mml_presentation)
        ml.markup=mml_tag(
            "mrow",left.markup+"<mo lspace='thickmathspace' rspace='thickmathspace'>mod</mo>"+
                   right.markup,idc);
      if (tex)
        ml.latex=left.latex+"\\;\\mathrm{mod}\\;"+right.latex;
      return ml;
    }
    if (g.is_symb_of_sommet(at_compose) && vectarg && isbinary) {
      ml.priority=_PRIORITY_APPLY;
      string op="&compfn;",opc="<compose/>",opt="\\circ ";
      assoc2markup(flatten_operands(g),ml,op,opc,opt,flags,idc,contextptr);
      return ml;
    }
    if (g.is_symb_of_sommet(at_composepow) && vectarg && isbinary) {
      const vecteur &args=*g._SYMBptr->feuille._VECTptr;
      ml.priority=_PRIORITY_EXP;
      left=gen2markup(args.front(),flags,idc,contextptr);
      prepend_minus(left,flags);
      if (left.priority>=ml.priority)
        parenthesize(left,flags);
      right=gen2markup(args.back(),flags,idc,contextptr);
      prepend_minus(right,flags);
      if (mml_content)
        ml.content=mml_tag("apply","<ci>@@</ci>"+left.content+right.content,++idc);
      if (mml_presentation)
        ml.markup=mml_tag("msup",left.markup+"<mrow><mo>&compfn;</mo>"+right.markup+"</mrow>",idc);
      if (tex)
        ml.latex=left.latex+"^{\\circ "+right.latex+"}";
      return ml;
    }
    if (g.is_symb_of_sommet(at_program) && vectarg && g._SYMBptr->feuille._VECTptr->size()==3) {
      const vecteur &args=*g._SYMBptr->feuille._VECTptr;
      ml.priority=_PRIORITY_ASSGN;
      if (args[0].type==_VECT && args[0].subtype==_SEQ__VECT && args[0]._VECTptr->size()>1) {
        const vecteur &vars=*args[0]._VECTptr;
        for (const_iterateur it=vars.begin();it!=vars.end();++it) {
          tmp=gen2markup(*it,flags,idc,contextptr);
          prepend_minus(tmp,flags);
          if (mml_content)
            ml.content+=mml_tag("bvar",tmp.content);
          if (mml_presentation)
            ml.markup+=tmp.markup;
          if (tex)
            ml.latex+=(it!=vars.begin()?",":"")+tmp.latex;
        }
        tmp=gen2markup(args[2],flags,idc,contextptr);
        prepend_minus(tmp,flags);
        if (tmp.priority>=ml.priority)
          parenthesize(tmp,flags);
        if (mml_content)
          ml.content=mml_tag("lambda",ml.content+tmp.content,++idc);
        if (mml_presentation)
          ml.markup=mml_tag("mrow",mml_tag("mfenced",ml.markup)+"<mo>&mapsto;</mo>"+tmp.markup,idc);
        if (tex)
          ml.latex="\\left("+ml.latex+"\\right)\\mapsto "+tmp.latex;
      } else {
        get_leftright(
          makevecteur(args[0].type==_VECT?args[0]._VECTptr->front():args[0],args[2]),
          &ml,left,right,flags,idc,contextptr);
        if (mml_content)
          ml.content=mml_tag("lambda","<bvar>"+left.content+"</bvar>"+right.content,++idc);
        if (mml_presentation)
          ml.markup=mml_tag("mrow",left.markup+"<mo>&mapsto;</mo>"+right.markup,idc);
        if (tex)
          ml.latex=left.latex+"\\mapsto "+right.latex;
      }
      return ml;
    }
    if (g.is_symb_of_sommet(at_tran) || g.is_symb_of_sommet(at_transpose) || g.is_symb_of_sommet(at_trn)){
      ml.priority=_PRIORITY_EXP;
      tmp=gen2markup(g._SYMBptr->feuille,flags,idc,contextptr);
      ml.neg=tmp.neg;
      if (tmp.priority>=ml.priority)
        parenthesize(tmp,flags);
      if (mml_content)
        ml.content=mml_tag("apply",(g.is_symb_of_sommet(at_trn)?mml_tag("ci","trn",0,"type","function")
                                                               :"<transpose/>")+tmp.content,++idc);
      if (mml_presentation)
        ml.markup=mml_tag(
            "msup",tmp.markup+(g.is_symb_of_sommet(at_trn)?"<mi>&#x2217;</mi>"
                                                          :"<mi mathvariant='sans-serif'>T</mi>"),idc);
      if (tex)
        ml.latex=tmp.latex+"^{"+(g.is_symb_of_sommet(at_trn)?"\\ast":"\\mathsf{T}")+"}";
      return ml;
    }
    if (g.is_symb_of_sommet(at_increment) || g.is_symb_of_sommet(at_decrement)) {
      ml.priority=_PRIORITY_UNARY;
      tmp=gen2markup(g._SYMBptr->feuille,flags,idc,contextptr);
      prepend_minus(tmp,flags);
      if (tmp.priority>=ml.priority)
        parenthesize(tmp,flags);
      if (mml_content)
        ml.content=mml_tag(
            "apply",(g.is_symb_of_sommet(at_increment)?"<ci>++</ci>":"<ci>--</ci>")+tmp.content,++idc);
      if (mml_presentation)
        ml.markup=mml_tag(
          "mrow",tmp.markup+"<mo mathsize='small'>"+
                 string(g.is_symb_of_sommet(at_increment)?"++":"&minus;&minus;")+"</mo>",idc);
      if (tex)
        ml.latex=tmp.latex+"\\mathclose{"+string(g.is_symb_of_sommet(at_increment)?"++}":"--}");
      return ml;
    }
    if ((g.is_symb_of_sommet(at_when) || g.is_symb_of_sommet(at_piecewise)) &&
        vectarg && g._SYMBptr->feuille._VECTptr->size()>=3) {
      const vecteur &args=*g._SYMBptr->feuille._VECTptr;
      ml.priority=_PRIORITY_COND;
      MarkupBlock otw;
      int nargs=args.size();
      bool has_otherwise=(nargs%2)!=0;
      if (has_otherwise) {
        otw=gen2markup(args.back(),flags,idc,contextptr);
        prepend_minus(otw,flags);
      }
      for (int i=0;i<nargs/2;++i) {
        get_leftright(makevecteur(args[2*i+1],args[2*i]),&ml,left,right,flags,idc,contextptr);
        if (mml_content)
          ml.content+=mml_tag("piece",left.content+right.content);
        if (mml_presentation)
          ml.markup+=mml_tag("mtr",mml_tag("mtd",left.markup+"<mo>,</mo>")+
                                   mml_tag("mtd",right.markup));
        if (tex)
          ml.latex+=left.latex+",&"+right.latex+"\\\\";
      }
      if (has_otherwise) {
        if (mml_content)
          ml.content+=mml_tag("otherwise",otw.content);
        if (mml_presentation)
          ml.markup+=mml_tag("mtr",mml_tag("mtd",otw.markup+"<mo>,</mo>")+
                                   mml_tag("mtd","<mi mathvariant='normal'>otherwise</mi>"));
        if (tex)
          ml.latex+=otw.latex+",&\\text{otherwise}";
      }
      if (mml_content)
        ml.content=mml_tag("piecewise",ml.content,++idc);
      if (mml_presentation)
        ml.markup=mml_tag("mfenced",mml_tag("mtable",ml.markup,0,"columnalign","left"),
                          idc,"open","{","close"," ");
      if (tex)
        ml.latex="\\begin{cases}"+ml.latex+"\\end{cases}";
      return ml;
    }
    if (g.is_symb_of_sommet(at_factorial)) {
      ml.priority=_PRIORITY_UNARY;
      ml.type=_MLBLOCK_FACTORIAL;
      tmp=gen2markup(g._SYMBptr->feuille,flags,idc,contextptr);
      prepend_minus(tmp,flags);
      if (tmp.priority>=ml.priority)
        parenthesize(tmp,flags);
      if (mml_content)
        ml.content=mml_tag("apply","<factorial/>"+tmp.content,++idc);
      if (mml_presentation)
        ml.markup=mml_tag("mrow",tmp.markup+"<mo>!</mo>",idc);
      if (tex)
        ml.latex=tmp.latex+"\\mathclose{!}";
      return ml;
    }
    if ((g.is_symb_of_sommet(at_binomial) || g.is_symb_of_sommet(at_nCr) ||
         g.is_symb_of_sommet(at_comb)) && vectarg &&
        isbinary) {
      const vecteur &args=*g._SYMBptr->feuille._VECTptr;
      left=gen2markup(args.front(),flags,idc,contextptr);
      prepend_minus(left,flags);
      right=gen2markup(args.back(),flags,idc,contextptr);
      prepend_minus(right,flags);
      if (mml_content)
        ml.content=mml_tag("apply",mml_csymbol("binomial","combinat1")+left.content+right.content,++idc);
      if (mml_presentation)
        ml.markup=mml_tag("mfenced","<mfrac linethickness='0'>"+left.markup+right.markup+"</mfrac>",idc);
      if (tex)
        ml.latex="\\binom{"+left.latex+"}{"+right.latex+"}";
      return ml;
    }
    if (g.is_symb_of_sommet(at_l1norm) || g.is_symb_of_sommet(at_l2norm) ||
        g.is_symb_of_sommet(at_norm) || g.is_symb_of_sommet(at_maxnorm) ||
        g.is_symb_of_sommet(at_matrix_norm)) {
      tmp=gen2markup(vectarg?g._SYMBptr->feuille._VECTptr->front():g._SYMBptr->feuille,
                     flags,idc,contextptr);
      prepend_minus(tmp,flags);
      MarkupBlock d;
      int n=-2;
      if (g.is_symb_of_sommet(at_maxnorm) || (g.is_symb_of_sommet(at_matrix_norm) && vectarg &&
                              g._SYMBptr->feuille._VECTptr->size()>1)) {
        if (g.is_symb_of_sommet(at_maxnorm) || is_inf(g._SYMBptr->feuille._VECTptr->at(1)))
          n=0;
        else {
          d=gen2markup(g._SYMBptr->feuille._VECTptr->at(1),flags,idc,contextptr);
          n=-1;
        }
      } else if (g.is_symb_of_sommet(at_l1norm))
        n=1;
      else if (g.is_symb_of_sommet(at_l2norm) || g.is_symb_of_sommet(at_norm))
        n=2;
      if (mml_content) {
        if (n<0 || n==1)
          ml.content=mml_tag("apply",mml_csymbol("L_norm","norm1")+
                             (n<0?d.content:mml_tag("ci","1",0,"type","integer"))+tmp.content,
                             ++idc);
        else
          ml.content=mml_tag("apply",mml_csymbol(n<1?
                                     "L_infinity_norm":"Euclidean_norm","norm1")+tmp.content,
                             ++idc);
      }
      if (mml_presentation)
        ml.markup="<mo>&#x2016;</mo>"+tmp.markup+"<mo>&#x2016;</mo>";
      if (tex)
        ml.latex="\\left\\|"+tmp.latex+"\\right\\|";
      if (n>-2) {
        string N=gen(n).print(contextptr);
        if (mml_presentation)
          ml.markup=mml_tag("msub","<mrow>"+ml.markup+"</mrow>"+
                                   (n<0?d.markup:(n<1?"<mi>&infin;</mi>":mml_tag("mn",N))),idc);
        if (tex)
          ml.latex+="_{"+(n<0?d.latex:(n<1?"\\infty":N))+"}";
      } else
        ml.markup=mml_tag("mrow",ml.markup,idc);
      return ml;
    }
    if (g.is_symb_of_sommet(at_logb) && vectarg && isbinary) {
      get_leftright(g._SYMBptr->feuille,NULL,left,right,flags,idc,contextptr);
      parenthesize(left,flags);
      ml.priority=_PRIORITY_APPLY;
      if (mml_content)
        ml.content=mml_tag(
            "apply","<log/>"+mml_tag("logbase",right.content)+left.content,++idc);
      if (mml_presentation)
        ml.markup=mml_tag(
            "mrow",mml_tag("msub","<mi>log</mi>"+right.markup)+left.markup,idc);
      if (tex)
        ml.latex="\\mathop{\\log_{"+right.latex+"}}"+left.latex;
      return ml;
    }
    if (g.is_symb_of_sommet(at_unit) && vectarg && isbinary) {
      const vecteur &args=*g._SYMBptr->feuille._VECTptr;
      left=gen2markup(args.front(),flags,idc,contextptr);
      right=gen2markup(args.back(),flags | _MARKUP_UNIT,idc,contextptr);
      ml.priority=_PRIORITY_MUL;
      ml.neg=(left.neg!=right.neg);
      if (mml_content)
        ml.content=mml_tag("apply","<times/>"+left.content+right.content,++idc);
      if (mml_presentation)
        ml.markup=mml_tag("mrow",
            left.markup+mml_tag("mo",mml_itimes,0,"rspace","thickmathspace")+right.markup,idc);
      if (tex)
        ml.latex=left.latex+"\\;"+right.latex;
      return ml;
    }
    if (g.is_symb_of_sommet(at_at) && vectarg && isbinary) {
      const vecteur &args=*g._SYMBptr->feuille._VECTptr;
      ml.priority=_PRIORITY_APPLY;
      ml.type=_MLBLOCK_HAS_SUBSCRIPT;
      tmp=gen2markup(args.front(),flags,idc,contextptr);
      prepend_minus(tmp,flags);
      if (tmp.priority>ml.priority || tmp.ctype(_MLBLOCK_HAS_SUBSCRIPT))
        parenthesize(tmp,flags);
      MarkupBlock sub;
      if (args.back().type==_VECT && args.back().subtype==_SEQ__VECT) {
        const vecteur &idx=*args.back()._VECTptr;
        string sep=mml_icomma,sept=" ";
        for (const_iterateur it=idx.begin();it!=idx.end();++it) {
          if ((!it->is_integer() || it->val<1 || it->val>9) &&
              (it->type!=_IDNT ||
                (((str=it->print(contextptr)).length()!=1 || !isalpha(str[0])) &&
                 !is_greek_letter(str)))) {
            sep="<mo>,</mo>";
            sept=",";
            break;
          }
        }
        for (const_iterateur it=idx.begin();it!=idx.end();++it) {
          sub=gen2markup(*it,flags,idc,contextptr);
          prepend_minus(sub,flags);
          if (mml_content)
            ml.content+=sub.content;
          if (mml_presentation)
            ml.markup+=(it!=idx.begin()?sep:"")+sub.markup;
          if (tex)
            ml.latex+=(it!=idx.begin()?sept:"")+sub.latex;
        }
        if (mml_content)
          ml.content=mml_tag("apply","<selector/>"+tmp.content+ml.content,++idc);
        if (mml_presentation)
          ml.markup=mml_tag("msub",tmp.markup+mml_tag("mrow",ml.markup),idc);
        if (tex)
          ml.latex=tmp.latex+"_{"+ml.latex+"}";
      } else {
        sub=gen2markup(args.back(),flags,idc,contextptr);
        prepend_minus(sub,flags);
        if (mml_content)
          ml.content=mml_tag("apply","<selector/>"+tmp.content+sub.content,++idc);
        if (mml_presentation)
          ml.markup=mml_tag("msub",tmp.markup+sub.markup,idc);
        if (tex)
          ml.latex=tmp.latex+"_{"+sub.latex+"}";
      }
      return ml;
    }
    if (dynamic_cast<const unary_function_user *>(g._SYMBptr->sommet.ptr())!=0 && vectarg &&
        isbinary) {
      ml.priority=_PRIORITY_APPLY;
      get_leftright(g._SYMBptr->feuille,&ml,left,right,flags,idc,contextptr);
      string op(g._SYMBptr->sommet.ptr()->s),opt;
      if (mml_content)
        ml.content=mml_tag("apply",mml_tag("ci",op)+left.content+right.content,++idc);
      if (op.length()==1 && !isalpha(op[0]) && op[0]!='_') {
        op="<mo>"+op+"</mo>";
        opt="\\mathbin{"+op+"}";
      } else {
        op="<mspace width='thickmathspace'/>"+idnt2markup(op,false,false,idc)+
             "<mspace width='thickmathspace'/>";
        opt="\\;"+idnt2markup(op,true,false)+"\\;";
      }
      if (mml_presentation)
        ml.markup=mml_tag("mrow",left.markup+op+right.markup,idc);
      if (tex)
        ml.latex=left.latex+opt+right.latex;
      return ml;
    }
    if (g.is_symb_of_sommet(at_local) || g.is_symb_of_sommet(at_ifte) ||
        g.is_symb_of_sommet(at_evalb) || g.is_symb_of_sommet(at_maple_if) ||
        g.is_symb_of_sommet(at_for) || g.is_symb_of_sommet(at_return) ||
        g.is_symb_of_sommet(at_try_catch) || g.is_symb_of_sommet(at_case) ||
        g.is_symb_of_sommet(at_throw) || g.is_symb_of_sommet(at_halt) ||
        g.is_symb_of_sommet(at_break) || g.is_symb_of_sommet(at_continue) ||
        g.is_symb_of_sommet(at_goto) || g.is_symb_of_sommet(at_si) ||
        g.is_symb_of_sommet(at_pour) || g.is_symb_of_sommet(at_tantque)) {
      flags|=_MARKUP_CODE;
      string2markup(ml,g.print(contextptr),flags,idc);
      ml.priority=_PRIORITY_OTHER;
      return ml;
    }
    if (g.is_symb_of_sommet(at_pnt))
      break;
    // general SYMB
    ml.priority=_PRIORITY_APPLY;
    if (g.is_symb_of_sommet(at_of)) {
      const vecteur &args=*g._SYMBptr->feuille._VECTptr;
      tmp=gen2markup(args.front(),flags,idc,contextptr);
      prepend_minus(tmp,flags);
      if (!tmp.ctype(_MLBLOCK_DERIVATIVE) && tmp.priority>=ml.priority)
        parenthesize(tmp,flags);
      if (mml_content)
        ml.content=tmp.content;
      if (mml_presentation)
        ml.markup=tmp.markup+mml_apply;
      if (tex)
        ml.latex=tmp.latex;
      gp=&args.back();
    } else {
      gp=&g._SYMBptr->feuille;
      if (mml_content)
        ml.content=func2markup(g,false,true,++idc);
      if (mml_presentation)
        ml.markup=func2markup(g,false,false,idc);
      if (tex)
        ml.latex=func2markup(g,true,false);
    }
    if (gp->type==_VECT && gp->subtype==_SEQ__VECT) {
      string vs,vsc,vst;
      for (const_iterateur it=gp->_VECTptr->begin();it!=gp->_VECTptr->end();++it) {
        tmp=gen2markup(*it,flags,idc,contextptr);
        if (mml_content)
          vsc+=tmp.content;
        if (mml_presentation)
          vs+=tmp.markup;
        if (tex)
          vst+=(it!=gp->_VECTptr->begin()?",":"")+tmp.latex;
      }
      if (mml_content)
        ml.content=mml_tag("apply",ml.content+vsc,++idc);
      if (mml_presentation)
        ml.markup=mml_tag("mrow",ml.markup+mml_tag("mfenced",vs),idc);
      if (tex)
        ml.latex+="\\left("+vst+"\\right)";
    } else {
      tmp=gen2markup(*gp,flags,idc,contextptr);
      prepend_minus(tmp,flags);
      if ((mml_presentation || tex) && is_elemfunc(g)) {
        ml.type=_MLBLOCK_ELEMAPP;
        ml.split_pos=ml.markup.length();
        ml.split_pos_tex=ml.latex.length();
        ml.appl=(tmp.priority==0 && !tmp.ctype(_MLBLOCK_NUMERIC_EXACT) &&
                   !tmp.ctype(_MLBLOCK_NUMERIC_APPROX) && !tmp.ctype(_MLBLOCK_SUBTYPE_IDNT));
      }
      if (!ml.appl)
        parenthesize(tmp,flags);
      if (mml_content)
        ml.content=mml_tag("apply",ml.content+tmp.content,++idc);
      if (mml_presentation)
        ml.markup=mml_tag("mrow",ml.markup+mml_apply+tmp.markup,idc);
      if (tex)
        ml.latex+=tmp.latex;
    }
    return ml;
  default:
    break;
  }
  // unsupported gen, call legacy conversion routines
  ml.priority=_PRIORITY_OTHER;
  if (tex)
    ml.latex=gen2tex(g,contextptr);
  if (mml_content)
    ml.content=mml_tag("cerror",mml_tag("csymbol","ExpressionNotSupported"),++idc);
  if (mml_presentation)
    ml.markup=insert_id(gen2mathml(g,contextptr),idc,false);
  return ml;
}

string export_latex(const gen &g,GIAC_CONTEXT) {
  MarkupBlock ml;
  int idc=0,flags=_MARKUP_TOPLEVEL | _MARKUP_ELEMPOW | _MARKUP_LATEX;
  ml=gen2markup(g,flags,idc,contextptr);
  prepend_minus(ml,flags);
  return ml.latex;
}

// XML pretty printing
#define XML_INDENT 2
const string xml_element_name_end=" \t\n\r>";
string trim_string(const string &s) {
  size_t start,end;
  for (start=0;start<s.size() && isspace(s[start]);start++);
  for (end=s.size();end-->0 && isspace(s[end]););
  return s.substr(start,end-start+1);
}
bool xml_read_element(const string &xml,size_t &pos,string &element,string &attrib,string &content) {
  while (pos<xml.size() && isspace(xml[pos])) ++pos;
  if (pos==xml.size()) {
    element=attrib=content="";
    return true;
  }
  if (xml[pos]!='<') return false;
  size_t start=pos+1;
  if (is_prefix(xml.substr(start),"--!")) {
    pos=xml.find("-->",start);
    if (pos==string::npos) return false;
    element="";
    content=xml.substr(start+3,pos-start-3);
    pos+=3;
    return true;
  }
  while (xml_element_name_end.find(xml[++pos])==string::npos) {
    if (pos+1==xml.size()) return false;
  }
  string e=xml.substr(start,pos-start);
  size_t astart=pos;
  while (xml[pos++]!='>') {
    if (pos==xml.size()) return false;
  }
  if (xml[pos-2]=='/') {
    content=attrib="";
    if (is_suffix(e,"/"))
      e="/"+e.substr(0,e.size()-1);
    else {
      attrib=trim_string(xml.substr(astart,pos-astart-2));
      e="/"+e;
    }
  } else if (xml[pos-2]=='?' && e=="?xml" && pos-astart-2>0)
    attrib=trim_string(xml.substr(astart,pos-astart-2));
  else {
    size_t p=pos,cstart=pos,q;
    int level=1;
    while(p<xml.size()) {
      if (xml[p]=='<') {
        q=xml.find(">",p+1);
        if (q==string::npos) return false;
        if (xml[q-1]=='/') {
          p=q+1;
          continue;
        }
        if (xml[p+1]=='/') {
          level--;
          if (level==0 && is_prefix(xml.substr(p+2),e) &&
              xml_element_name_end.find(xml[p+2+e.size()])!=string::npos) {
            pos=q+1;
            break;
          }
        } else level++;
        p=q;
      }
      ++p;
    }
    if(p==xml.size()) return false;
    attrib=trim_string(xml.substr(astart,cstart-astart-1));
    content=trim_string(xml.substr(cstart,p-cstart));
  }
  element=e;
  return true;
}
string xml_pretty_print(const string &xml,int level=0) {
  size_t pos=0;
  string element,attrib,content,indent=string(level*XML_INDENT,32),ret="";
  while(pos!=xml.size()) {
    if (!xml_read_element(xml,pos,element,attrib,content)) return xml;
    if (!attrib.empty()) attrib=" "+attrib;
    if (element.empty() && content.empty()) continue;
    if (element.empty()) // the "element" is actually a comment
      ret+=indent+"<--!"+content+"-->\n";
    else if (element[0]=='?')
      ret+=indent+"<"+element+attrib+"?>\n";
    else if (element[0]=='/')
      ret+=indent+"<"+element.substr(1)+attrib+"/>\n";
    else
      ret+=indent+"<"+element+attrib+">"+xml_pretty_print(content,level+1)+"</"+element+">\n";
  }
  if (trim_string(ret).empty())
    return "";
  if (level==0 && ret.size()>0 && ret[ret.size()-1]=='\n')
    ret=ret.substr(0,ret.size()-1);
  return (level>0?"\n":"")+ret+(level>1?string((level-1)*XML_INDENT,32):"");
}

const string mathml_header_attributes="mode='display' xmlns='http://www.w3.org/1998/Math/MathML'";

string export_mathml_content(const gen &g,GIAC_CONTEXT) {
  MarkupBlock ml;
  int idc=0,flags=_MARKUP_TOPLEVEL | _MARKUP_MATHML_CONTENT;
  ml=gen2markup(g,flags,idc,contextptr);
  prepend_minus(ml,flags);
  return "<math "+mathml_header_attributes+">"+ml.content+"</math>";
}

string export_mathml_presentation(const gen &g,GIAC_CONTEXT) {
  MarkupBlock ml;
  int idc=0,flags=_MARKUP_TOPLEVEL | _MARKUP_ELEMPOW | _MARKUP_MATHML_PRESENTATION;
  ml=gen2markup(g,flags,idc,contextptr);
  prepend_minus(ml,flags);
  return "<math "+mathml_header_attributes+">"+ml.markup+"</math>";
}

string export_mathml(const gen &g,GIAC_CONTEXT) {
  MarkupBlock ml;
  int idc=0,flags=_MARKUP_TOPLEVEL | _MARKUP_MATHML_PRESENTATION | _MARKUP_MATHML_CONTENT;
  ml=gen2markup(g,flags,idc,contextptr);
  prepend_minus(ml,flags);
  return "<math "+mathml_header_attributes+"><semantics>"+ml.markup+
         "<annotation-xml encoding='MathML-Content'>"+ml.content+"</annotation-xml>"+
         "<annotation encoding='Giac'>"+str_to_mml(g.print(contextptr),false)+
         "</annotation></semantics></math>";
}

  #ifndef KHICAS
bool has_improved_latex_export(const gen &g,string &s,bool override_texmacs,GIAC_CONTEXT) {
  if (force_legacy_conversion_to_latex || g.is_symb_of_sommet(at_pnt))
    return false;
  switch (g.type) {
  case _POLY: case _SPOL1: case _EXT: case _ROOT:
  case _USER: case _EQW: case _GROB: case _POINTER_:
    return false;
  default:
    break;
  }
  bool use_texmacs_compatibility=is_texmacs_compatible_latex_export;
  if (override_texmacs)
    enable_texmacs_compatible_latex_export(false);
  force_legacy_conversion_to_latex=true;
  s=export_latex(g,contextptr);
  force_legacy_conversion_to_latex=false;
  if (override_texmacs)
    enable_texmacs_compatible_latex_export(use_texmacs_compatibility);
  return true;
}
#endif
  
gen _export_mathml(const gen &g,GIAC_CONTEXT) {
  if (g.type==_STRNG && g.subtype==-1) return g;
  gen e;
  int extype=0;
  if (g.type==_VECT && g.subtype==_SEQ__VECT) {
    const vecteur &args=*g._VECTptr;
    if (args.size()!=2) return gensizeerr(contextptr);
    e=args.front();
    if (args.back()==at_display)
      extype=1;
    else if (args.back()==at_content)
      extype=2;
    else return gensizeerr(contextptr);   
  } else e=g;
  string ret;
  switch (extype) {
  case 0:
    ret=export_mathml(e,contextptr);
    break;
  case 1:
    ret=export_mathml_presentation(e,contextptr);
    break;
  case 2:
    ret=export_mathml_content(e,contextptr);
    break;
  default:
    assert(false); // unreachable
  }
  return string2gen(ret,false);
}
static const char _export_mathml_s[]="export_mathml";
static define_unary_function_eval(__export_mathml,&_export_mathml,_export_mathml_s);
define_unary_function_ptr5(at_export_mathml,alias_at_export_mathml,&__export_mathml,0,true)

gen _xml_print(const gen &g,GIAC_CONTEXT) {
  if (g.type!=_STRNG)
    return gentypeerr(contextptr);
  if (g.subtype==-1)
    return g;
  string s=g.print(contextptr);
  s=s.substr(1,s.size()-2);
  return string2gen(xml_pretty_print(s),false);
}
static const char _xml_print_s[]="xml_print";
static define_unary_function_eval(__xml_print,&_xml_print,_xml_print_s);
define_unary_function_ptr5(at_xml_print,alias_at_xml_print,&__xml_print,0,true)

#ifndef NO_NAMESPACE_GIAC
} // namespace giac
#endif // ndef NO_NAMESPACE_GIAC
