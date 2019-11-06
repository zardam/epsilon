/*             itex2MML 1.3.3
 *   itex2MML.cc last modified by Jacques Distler on 11/21/2007
 *   forked by B. Parisse for hevea -mathjax support
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <map>
#include <string>
#include <fstream>

#include "itex2MML.h"

int debug=0;

using namespace std;

typedef map<string,int> ref_t;

string print(int i){
  string res;
  while (i){
    res = char('0'+(i%10))+res;
    i /= 10;
  }
  return res;
}

// convert \( \) to $..$ and collect labels in math \begin{equation/eqnarray}
// also in \[ \] if autonum_bracket is true
std::string todollar(std::string & s,ref_t & r,bool hevea,bool autonum_bracket,int & counter){
  std::string res,cur_label;
  size_t ss=s.size();
  bool math=false,array=false;
  if (ss<1) return s;
  for (size_t i=0;i<ss;++i){
    char c=s[i];
    if (c=='$'){
      if (s[i+1]=='$'){
	math=!math;
	res += "$$";
	++i;
	continue;
      }
      if (hevea){ // hevea -mathjax outputs \( \) for inline math
	res += "&#36;";
	continue;
      }
      math=!math;
    }
    if (math && hevea && c=='<'){
      res += "&lt;";
      continue;
    }
    if (math && hevea && c=='>'){
      res += "&gt;";
      continue;
    }
    if (math && i+5<ss && c=='&'){
      string tmp=s.substr(i,5);
      if (tmp=="&amp;"){ // ignored
	res += " & ";
	i+=4;
	continue;
      }
    }
    if (c=='\\'){
      char d=i+1<ss?s[i+1]:0;
      if (math && !array && d=='\\'){
	res += "\\quad ";
	++i;
	continue;
      }
      if (d=='(' || d==')'){
	math=(d=='(');
	++i;
	res += '$';
	continue;
      }
      if (d=='['){
	math=true;
	if (autonum_bracket){
	  ++counter;
	  res += "<a id=\"eqref_";
	  res += print(counter);
	  res += "\"></a>";
	}
	cur_label="";
      }
      if (d==']'){
	math=false;
	if (autonum_bracket){
	  if (cur_label.size()){
	    r[cur_label]=counter;
	    cur_label="";
	  }
	  res += "\\quad ("+print(counter)+") ";
	  res += "\\]";
	  ++i;
	  continue;
	}
	else res+=' '; // additional space before \]
      }
      if (i+16<ss && d=='b'){ // search \begin{equation} or \begin{eqnarray}
	string tmp=s.substr(i,16);
	if (tmp=="\\begin{verbatim}"){
	  int epos=s.find("\\end{verbatim}",i);
	  if (epos>i && epos<ss){
	    int delta=epos-i;
	    res += "<tt>"+s.substr(i+16,delta-16)+"</tt>";
	    i += delta+14;
	    continue;
	  }
	}
	bool eqnarray=(tmp=="\\begin{eqnarray}");
	if (tmp=="\\begin{equation}" || eqnarray){
	  math=true;
	  ++counter;
	  cur_label="";
	  res += "<a id=\"eqref_";
	  res += print(counter);
	  res += "\"></a>";
	  res += "\\[";
	  if (eqnarray)
	    res += " \\begin{matrix}";
	  i += 15;
	  continue;
	}
	tmp=s.substr(i,min(18,int(ss-i)));
	int pos1=tmp.find("array"), pos2=tmp.find("matrix");
	if (tmp.substr(0,6)=="\\begin" && ( (pos1>0 && pos1<16) || (pos2>0 && pos2<16)))
	  array=true;
      }
      if (i+17<ss && d=='b'){ // search \begin{equation*} or \begin{eqnarray*}
	string tmp=s.substr(i,17);
	bool eqnarray=(tmp=="\\begin{eqnarray*}");
	if (tmp=="\\begin{equation*}" || eqnarray){
	  math=true;
	  cur_label="";
	  if (eqnarray)
	    res += "$$ \\begin{matrix}";
	  else
	    res += "$$";
	  i += 16;
	  continue;
	}
      }
      if (i+14<ss && d=='e'){ // \end{equation} or \end{eqnarray}
	string tmp=s.substr(i,14);
	bool eqnarray=(tmp=="\\end{eqnarray}");
	if (tmp=="\\end{equation}" || eqnarray){
	  math=false;
	  if (cur_label.size()){
	    r[cur_label]=counter;
	    cur_label="";
	  }
	  if (eqnarray)
	    res += "\\end{matrix} ";
	  res += "\\qquad ("+print(counter)+") ";
	  res += "\\]";
	  i += 13;
	  continue;
	}
	tmp=s.substr(i,min(16,int(ss-i)));
	int pos1=tmp.find("array"), pos2=tmp.find("matrix");
	if (tmp.substr(0,4)=="\\end" && ( (pos1>0 && pos1<16) || (pos2>0 && pos2<16)))
	  array=true;
      }
      if (i+15<ss && d=='e'){ // \end{equation*} or \end{eqnarray*}
	string tmp=s.substr(i,15);
	bool eqnarray=(tmp=="\\end{eqnarray*}");
	if (tmp=="\\end{equation*}" || eqnarray){
	  math=false;
	  cur_label="";
	  if (eqnarray)
	    res += " \\end{matrix}";
	  res += " $$";
	  i += 14;
	  continue;
	}
      }
      if (i+6<ss && d=='l'){ // \label
	string tmp=s.substr(i,7);
	if (tmp=="\\label{"){
	  if (math && cur_label.size()){
	    r[cur_label]=counter;
	    // don't ++counter; 
	    // all equations in the eqnarray are in a matrix 
	    // and share the same number
	  }
	  cur_label="";
	  i+=7;
	  for (;i<ss;++i){
	    if (s[i]=='}') break;
	    //if (isalnum(s[i]))
	      cur_label += s[i];
	  }
	  //res += tmp + cur_label + '}';
	  continue;
	}
      }
    }
    res += c;
  }
  if (debug){
    cerr << "Cross-references in math environments" << endl;
    ref_t::const_iterator it=r.begin(),itend=r.end();
    for (;it!=itend;++it)
      cerr << it->first << ":" << it->second << endl;
  }
  return res;
}

// replace \ref{} using labels in r, removing $..$
string crossref(const string & s,const string & refcmd,const ref_t & r){
  size_t i,ss=s.size(),rs=refcmd.size();
  string res;
  for (i=0;i+rs<ss;){
    if (s[i]!=refcmd[0]){
      res += s[i];
      ++i;
      continue;
    }
    string tmp=s.substr(i,rs);
    if (tmp!=refcmd){
      res += s[i];
      ++i;
      continue;      
    }
    i += rs;
    string cur_label;
    for (;i<ss;++i){
      if (s[i]=='}') break;
      //if (isalnum(s[i]))
	cur_label += s[i];
    }
    ref_t::const_iterator it=r.find(cur_label),itend=r.end();
    if (it==itend){
      res += cur_label;
      cerr << "Undefined cross-reference " << cur_label << endl;
      ++i;
      continue;
    }
    int n=it->second;
    bool skip=false;
    if (res.size() && res[res.size()-1]=='$'){
      skip=true;
      res=res.substr(0,res.size()-1);
    }
    res += "<a href=\"#eqref_";
    res += print(n);
    res += "\">";
    res += print(n);
    res += "</a>";
    ++i;
    if (skip && i<ss && s[i]=='$')
      ++i;
    continue;
  }
  for (;i<ss;++i)
    res += s[i];
  return res;
}

int main (int argc, char ** argv)
{
  bool bPrintItex = false;
  bool bRawFilter = false;
  bool bInline    = false;
  bool bDisplay   = false;

  bool bStop = false;
  bool bForbidMarkup = true;
  bool hevea=true;
  const char * filein=0;
  const char * fileout=0;
  for (int arg = 1; arg < argc; arg++){
    std::string args = argv[arg];
    if (args[0]!='-'){
      if (filein)
	fileout=argv[arg];
      else 
	filein=argv[arg];
      continue;
    }
    if (args=="--debug" || args=="-d")
      debug=1;
    if (args=="--nohevea" )
      hevea=false;
    else
      bForbidMarkup = false;
    if (args == "--version" || args == "-v"){
      cout << "hevea2mml " << ITEX2MML_VERSION << endl <<
	"Forked from itex2MML\n See http://golem.ph.utexas.edu/~distler/blog/itex2MML.html for more information" << endl;
      cout << "Licensed under GPL version 3" << endl;
      bStop = true;
      break;
    }

    if (args == "--help" || args == "-h"){
      fputs ("usage: itex2MML [OPTIONS]\n"
	     "\n"
	     "itex2MML filters an input text stream (e.g., an XHTML web page) converting itex expressions\n"
	     "to MathML. Inline itex expressions are delimited either side by single dollar symbols ($):\n"
	     "\n"
	     "\t<p>The parameters $\\alpha$ and $\\beta$ in the function $f(x)$ are defined below.</p>\n"
	     "\n"
	     "For normal display of equations, etc., itex expressions can be delimited with double dollar\n"
	     "symbols ($$) either side or by \\[ to the left and \\] to the right:\n"
	     "\n"
	     "\t<p class=\"equation\">\\[\n"
	     "\t\tf(x) = \\alpha x + \\frac{\\beta}{1+|x|}\n"
	     "\t\\]</p>\n"
	     "\n"
	     "itex2MML Options:\n"
	     "\n"
	     "  --raw-filter    filter input stream, converting equations as found to MathML [stops on error]\n"
	     "  --inline        converts a single itex equation, without any $ symbols, to inline MathML\n"
	     "  --display       converts a single itex equation, without any $ symbols, to display-mode MathML\n"
	     "  --forbid-markup forbid markup (more precisely, the '<' and '>' characters) in itex equations\n"
	     "  --print-itex    used in conjuction with --inline or --display: prints the itex string\n"
	     "\n"
	     "For further information, see http://golem.ph.utexas.edu/~distler/blog/itex2MML.html\n", stdout);

      bStop = true;
      break;
    }
    if (args == "--print-itex"){
      bPrintItex = true;
      bRawFilter = false;
      continue;
    }
    if (args == "--forbid-markup"){
      bRawFilter = false;
      bForbidMarkup = true;
      continue;
    }
    if (args == "--inline"){
      bRawFilter = false;
      bInline    = true;
      bDisplay   = false;
      continue;
    }
    if (args == "--display"){
      bRawFilter = false;
      bInline    = false;
      bDisplay   = true;
      continue;
    }
    if (args == "--raw-filter"){
      bRawFilter = true;
      bPrintItex = false;
      bInline    = false;
      bDisplay   = false;
      continue;
    }
  }
  if (bStop) return 0;

  std::string itex;

  if (bInline)  itex += "$";
  if (bDisplay) itex += "$$";

#define BUFSIZE 65536
  char buffer[BUFSIZE];
  FILE * f =0;
  string fs;
  if (filein){
    fs=filein;
    if (fs.size()>=4 && fs.substr(fs.size()-4,4)==".tex"){
      std::string cmd="hevea -fix "+fs;
      cout << "Running " << cmd << endl;
      system(cmd.c_str());
      fs=fs.substr(0,fs.size()-4)+".html";
      fileout=filein=fs.c_str();
      f=fopen(filein,"r");
      if (!f)
	return 1;
    }
    else {
      fs += string(".html");
      f=fopen(filein,"r");
      if (!f){
	f=fopen(fs.c_str(),"r");
	if (f){
	  if (!fileout)
	    fileout=fs.c_str();
	}
	else {
	  cerr << "Unable to open file " << filein << endl;
	  return 1;
	}
      }
    }
  }
  if (!f)
    f=stdin;
  while (fgets (buffer, BUFSIZE, f)) itex += buffer;    
  if (bInline)  itex += "$";
  if (bDisplay) itex += "$$";

  if (bPrintItex){
    fputs (itex.c_str (), stdout);
    fputs ("\n", stdout);
    fflush (stdout);
  }
  // convert \( and \) to $ and compute crossref
  ref_t r;
  int counter=0;
  itex=todollar(itex,r,hevea,false,counter);
  // apply crossref
  itex=crossref(itex,"\\ref{",r);
  itex=crossref(itex,"\\eqref{",r);
  if (!bInline && !bDisplay){
    if (fileout){
      FILE * f =freopen(fileout,"w",stdout);
      if (!f){
	cerr << "Unable to write on file " << fileout << endl;
	return 1;
      }
    }
    if (bRawFilter)
      itex2MML_filter (itex.c_str(), itex.size());
    else
      if (bForbidMarkup)
	itex2MML_strict_html_filter (itex.c_str(), itex.size());
      else
	itex2MML_html_filter (itex.c_str(), itex.size());
    return 0;
  }

  char * mathml = itex2MML_parse (itex.c_str(), itex.size());

  if (mathml){
    if (fileout){
      ofstream of(fileout);
      of << mathml << endl;
    }
    else {
      fputs (mathml, stdout);
      fputs ("\n", stdout);
    }
    itex2MML_free_string (mathml);
    mathml = 0;
  }
  else{
    fputs ("itex2MML: itex parser failed to generate MathML from itex!\n", stderr);
  }
  return 0;
}
