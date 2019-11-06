// -*- mode:C++, compile-command: "g++ -g translate.cc" -*-

using namespace std;
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <string.h>

  struct ltstring
  {
    bool operator()(const std::string & s1, const std::string & s2) const
    {
      return strcmp(s1.c_str(), s2.c_str()) < 0;
    }
  };

typedef map<string,vector<string>,ltstring > maps;
// format: index is the basename msgid, vector<string> has size langs
// and contains the translation for each lang

const char * lang [] = { "en.po","fr.po"};
const int langs=sizeof(lang)/sizeof(char *);
const char * base_name="giac.pot_";
const char * output="aspen_translate.h";
const int BUFSIZE=1024;
char buf[BUFSIZE+1]; 

string extract(const string & s){
  int l=s.size();
  int pos1=s.find('"');
  if (pos1<0 || pos1>l-2)
    return "";
  int pos2=s.find('"',pos1+1);
  if (pos2<=pos1+1 || pos2>=l)
    return "";
  string res=s.substr(pos1+1,pos2-pos1-1);
  while (!res.empty() && res[res.size()-1]=='\\')
    res=res.substr(0,res.size()-1);
  return res;
}

bool next_record(ifstream & in,string & msgid,string & msgstr){
  msgid.clear();
  msgstr.clear();
  int status=-1;
  string s;
  while (!in.eof()){
    in.getline(buf,BUFSIZE,'\n');
    int l=strlen(buf);
    if (l<2){
      status=0;
      if (!msgid.empty())
	return true;
      msgid.clear();
      msgstr.clear();
      continue;
    }
    if (buf[0]=='#'){ // ignore except #, fuzzy
      if (strlen(buf)>4 && buf[1]==',' && buf[3]=='f')
	status = 1;
      continue;
    }
    if (status==1)
      continue;
    s=buf;
    if (l>6 && s.substr(0,5)=="msgid"){
      s=s.substr(6,s.size()-6);
      status =2;
    }
    if (l>7 && s.substr(0,6)=="msgstr"){
      s=s.substr(7,s.size()-7);
      status=3;
    }
    if (status==2)
      msgid += extract(s);
    if (status==3)
      msgstr += extract(s);
  }
  return false;
}

int main(){
  maps traduc;
  // read base_name to traduc
  string msgid,msgstr;
  ifstream in(base_name);
  while (next_record(in,msgid,msgstr)){
    cerr << msgid << endl;
    traduc[msgid]=vector<string>(langs);
  }
  in.close();
  // read PO translations, put in traduc 
  for (int i=0;i<langs;++i){
    cerr << "lang " << lang[i] << endl;
    ifstream inpo(lang[i]);
    while (next_record(inpo,msgid,msgstr)){
      maps::iterator it=traduc.find(msgid),itend=traduc.end();
      if (it==itend){
	cerr << "Invalid msgid in file " << lang[i] << " : " << msgid << endl;
	continue;
      }
      if (!msgstr.empty())
	it->second[i]=msgstr;
    }
  }
  // output sorted table of strings
  ofstream of(output);
  of << "typedef const char * const char4["<<langs+1<<"];"<<endl;
  of << "const char4 aspen_giac_translations [] = {" << endl;
  maps::iterator it=traduc.begin(),itend=traduc.end();
  int nrecords;
  for (nrecords=0;it!=itend;++nrecords){
    of << "{\"" << it->first << "\",";
    vector<string> & v = it->second;
    for (int i=0;i<langs;++i){
      if (v[i].empty())
	of << 0 ;
      else
	of << '"' << v[i] <<'"';
      of << ((i==langs-1)?"}":",");
    }
    ++it;
    if (it!=itend)
      of << ",\n";
  }
  of << "\n};" << endl;
  of << "const int aspen_giac_records=" << nrecords << ";" << endl;
  of.close();
  cerr << "Some strings are not correctly sorted, check and correct with test.cc" << endl;
  return 0;
}
