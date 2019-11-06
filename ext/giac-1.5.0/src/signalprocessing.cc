/*
 * signalprocessing.cc
 *
 * (c) 2018-2019 by Luka Marohnić
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include "giacPCH.h"
#include "giac.h"
#include "signalprocessing.h"

#ifdef HAVE_LIBSAMPLERATE
#include "samplerate.h"
#endif

using namespace std;

#ifndef NO_NAMESPACE_GIAC
namespace giac {
#endif // ndef NO_NAMESPACE_GIAC

int nextpow2(int n) {
    int m=1;
    while (m<n) m*=2;
    return m;
}

bool is_sound_data(const gen &g,int &nc,int &bd,int &sr,int &len) {
    if (g.type!=_VECT)
        return false;
    vecteur &gv=*g._VECTptr;
    if (gv.size()<2 || gv.front().type!=_VECT || gv.front()._VECTptr->size()!=4)
        return false;
    vecteur &header=*gv.front()._VECTptr;
    if (!is_integer_vecteur(header))
        return false;
    int n=header[0].val,b=header[1].val,s=header[2].val,l=(8*header[3].val)/(n*b);
    if (n<1 || (b!=8 && b!=16 && b!=24) || s<1 || l<0 ||
            int(gv.size())!=n+1)
        return false;
    for (const_iterateur it=gv.begin()+1;it!=gv.end();++it) {
        if (it->type!=_VECT)
            return false;
        if (int(it->_VECTptr->size())!=l)
            return false;
    }
    nc=n; bd=b; sr=s; len=l;
    return true;
}

vecteur decode_chdata(const vecteur &data,int bd,int start,int len) {
    if (data.empty() || (bd!=8 && bd!=16))
        return vecteur(0);
    vecteur res;
    int l=len<0?data.size():len;
    res.reserve(l);
    int v;
    double denom=std::pow(2,bd-1);
    for (const_iterateur it=data.begin()+start;it!=data.begin()+start+l;++it) {
        if (!it->is_integer())
            return vecteur(0);
        v=it->val;
        if (bd==8) v-=127;
        else if (bd==16 && v>=32768) v-=65536;
        res.push_back(double(v)/denom);
    }
    return res;
}

vecteur encode_chdata(const vecteur &data,int bd,double ratio,GIAC_CONTEXT) {
    if (data.empty() || (bd!=8 && bd!=16))
        return vecteur(0);
    vecteur res;
    res.reserve(data.size());
    double v,fac=std::pow(2.0,bd-1)-1.0;
    int k;
    for (const_iterateur it=data.begin();it!=data.end();++it) {
        if (!is_real(*it,contextptr))
            return vecteur(0);
        v=std::max(-1.0,std::min(1.0,ratio*_evalf(*it,contextptr).DOUBLE_val()));
        k=std::floor(fac*v);
        if (bd==8) k+=127;
        res.push_back(k);
    }
    return res;
}

int read_channel_data(const gen &g,int &nc,vector<vecteur*> &data) {
    int ret;
    if (ckmatrix(g)) {
        matrice &chns=*g._VECTptr;
        nc=chns.size();
        ret=chns.front()._VECTptr->size();
        for (const_iterateur jt=chns.begin();jt!=chns.end();++jt) {
            data.push_back(jt->_VECTptr);
        }
    } else {
        data.push_back(g._VECTptr);
        ret=g._VECTptr->size();
    }
    return ret;
}

gen _createwav(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    int len=0,bd=16,sr=44100,nc=1;
    double ratio=1.0,norm=-1;
    vector<vecteur*> data;
    if (g.is_integer()) {
        if (g.val<1)
            return gensizeerr(contextptr);
        len=g.val;
    } else if (g.type==_VECT) {
        if (g.subtype==_SEQ__VECT) {
            vecteur &args=*g._VECTptr;
            double secs=-1,plen=-1;
            for (const_iterateur it=args.begin();it!=args.end();++it) {
                if (it->is_symb_of_sommet(at_equal)) {
                    gen &lh=it->_SYMBptr->feuille._VECTptr->front();
                    gen &rh=it->_SYMBptr->feuille._VECTptr->back();
                    if (lh==at_channels) {
                        if (!rh.is_integer() || (nc=rh.val)<1)
                            return gensizeerr(contextptr);
                    } else if (lh==at_bit_depth) {
                        if (!rh.is_integer() || (bd=rh.val)<0 || (bd!=8 && bd!=16))
                            return gensizeerr(contextptr);
                    } else if (lh==at_samplerate) {
                        if (!rh.is_integer() || (sr=rh.val)<1)
                            return gensizeerr(contextptr);
                    } else if (lh==at_size) {
                        if (!rh.is_integer() || (plen=rh.val)<1)
                            return gensizeerr(contextptr);
                    } else if (lh==at_normalize) {
                        if (!is_real(rh,contextptr))
                            return gensizeerr(contextptr);
                        norm=-_evalf(rh,contextptr).DOUBLE_val();
                    } else if (lh==at_duration) {
                        if (!is_real(rh,contextptr))
                            return gensizeerr(contextptr);
                        secs=_evalf(rh,contextptr).DOUBLE_val();
                    } else if (lh==at_channel_data) {
                        if (rh.type!=_VECT)
                            return gensizeerr(contextptr);
                        plen=read_channel_data(rh,nc,data);
                    } else return gensizeerr(contextptr);
                } else if (it->type==_VECT)
                    plen=read_channel_data(*it,nc,data);
            }
            if (secs>=0)
                len=1+std::floor(secs*sr);
            else if ((len=plen)<0)
                return gensizeerr(contextptr);
        } else {
            if (is_sound_data(g,nc,bd,sr,len)) {
                for (int i=1;i<=nc;++i) {
                    data.push_back(g._VECTptr->at(i)._VECTptr);
                }
            } else len=read_channel_data(g,nc,data);
        }
    } else return gentypeerr(contextptr);
    if (!data.empty() && int(data.size())!=nc)
        return gendimerr(contextptr);
    if (len==0) {
        for (vector<vecteur*>::const_iterator it=data.begin();it!=data.end();++it) {
            len=std::max(len,(int)(*it)->size());
        }
    }
    if (norm>=0) {
        double maxamp=0;
        for (vector<vecteur*>::const_iterator it=data.begin();it!=data.end();++it) {
            maxamp=std::max(maxamp,_evalf(_max(_abs(**it,contextptr),contextptr),contextptr).DOUBLE_val());
        }
        ratio=std::pow(10.0,-norm/20.0)/maxamp;
    }
    /* create wav data */
    vecteur header,res;
    header.resize(4);
    header[0]=nc;
    header[1]=bd;
    header[2]=sr;
    header[3]=(bd*len*nc)/8;
    res.push_back(header);
    for (vector<vecteur*>::const_iterator it=data.begin();it!=data.end();++it) {
        vecteur enc=encode_chdata(**it,bd,ratio,contextptr);
        enc.resize(len,0);
        res.push_back(enc);
    }
    return res;
}
static const char _createwav_s []="createwav";
static define_unary_function_eval (__createwav,&_createwav,_createwav_s);
define_unary_function_ptr5(at_createwav,alias_at_createwav,&__createwav,0,true)

gen _plotwav(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT)
        return gentypeerr(contextptr);
    vecteur opts;
    if (g.subtype==_SEQ__VECT)
        opts=vecteur(g._VECTptr->begin()+1,g._VECTptr->end());
    const gen &wav=g.subtype==_SEQ__VECT?g._VECTptr->front():g;
    int nc,bd,sr,len,slice_start=0;
    if (!is_sound_data(wav,nc,bd,sr,len))
        return gentypeerr(contextptr);
    for (const_iterateur it=opts.begin();it!=opts.end();++it) {
        if (it->is_symb_of_sommet(at_equal)) {
            gen &lh=it->_SYMBptr->feuille._VECTptr->front();
            gen &rh=it->_SYMBptr->feuille._VECTptr->back();
            if (lh==at_range) {
                if (rh.is_symb_of_sommet(at_interval)) {
                    gen a=rh._SYMBptr->feuille._VECTptr->front();
                    gen b=rh._SYMBptr->feuille._VECTptr->back();
                    if (!is_real(a,contextptr) || !is_real(b,contextptr) || is_positive(a-b,contextptr))
                        return gensizeerr(contextptr);
                    slice_start=std::max(slice_start,_floor(a*gen(sr),contextptr).val);
                    len=std::min(len-slice_start,_floor(b*gen(sr),contextptr).val-slice_start);
                } else if (rh.type==_VECT) {
                    if (rh._VECTptr->size()!=2 || !rh._VECTptr->front().is_integer() ||
                            !rh._VECTptr->back().is_integer())
                        return gensizeerr(contextptr);
                    slice_start=std::max(slice_start,rh._VECTptr->front().val);
                    len=std::min(len-slice_start,rh._VECTptr->back().val-slice_start);
                } else return gensizeerr(contextptr);
            }
        }
    }
    if (len==0)
        return vecteur(0);
    double dur=double(len)/double(sr),width=1500.0,sc=double(len)/width,x,y=nc==1?0.0:1.0;
    double fmax,fmin,s,t0=double(slice_start)/double(sr),dt=1.0/sr;
    int step=std::floor(sc),bnd;
    vecteur drawing,tvec;
    if (sc<=1.0) tvec.resize(len);
    for (int c=1;c<=nc;++c) {
        vecteur data=decode_chdata(*wav._VECTptr->at(c)._VECTptr,bd,slice_start,len),unodes,lnodes;
        if (sc<=1.0) {
            for (iterateur it=data.begin();it!=data.end();++it) {
                *it+=gen(y);
                tvec[it-data.begin()]=t0+(it-data.begin())*dt;
            }
            drawing.push_back(_listplot(_zip(makesequence(at_makevector,tvec,data),contextptr),contextptr));
        } else {
            unodes.reserve(std::floor(width));
            lnodes.reserve(std::floor(width));
            for (int i=0;i<len;i+=step) {
                fmax=fmin=0.0;
                bnd=std::min(i+step,len);
                for (int j=i;j<bnd;++j) {
                    s=_evalf(data[j],contextptr).DOUBLE_val();
                    if (s>fmax) fmax=s;
                    else if (s<fmin) fmin=s;
                }
                x=t0+dur*(i/step)/width;
                unodes.push_back(makecomplex(x,y+fmax));
                lnodes.push_back(makecomplex(x,y+fmin));
            }
            std::reverse(lnodes.begin(),lnodes.end());
            vecteur nodes=mergevecteur(unodes,lnodes);
            nodes.push_back(symbolic(at_equal,makesequence(at_display,_FILL_POLYGON)));
            drawing.push_back(_polygone(change_subtype(nodes,_SEQ__VECT),contextptr));
            drawing.push_back(_segment(makesequence(makecomplex(t0,y),makecomplex(t0+dur,y)),contextptr));
        }
        y-=2.0;
    }
    return drawing;
}
static const char _plotwav_s []="plotwav";
static define_unary_function_eval (__plotwav,&_plotwav,_plotwav_s);
define_unary_function_ptr5(at_plotwav,alias_at_plotwav,&__plotwav,0,true)

gen _stereo2mono(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    int nc,bd,sr,len;
    if (!is_sound_data(g,nc,bd,sr,len))
        return gentypeerr(contextptr);
    if (nc==1)
        return g;
    vecteur data(len,0);
    for (int i=1;i<=nc;++i) {
        const vecteur &chan=*g._VECTptr->at(i)._VECTptr;
        data=addvecteur(data,decode_chdata(chan,bd));
    }
    data=multvecteur(fraction(1,nc),data);
    vecteur header=*g._VECTptr->front()._VECTptr,edata=encode_chdata(data,bd,1.0,contextptr);
    header[0]=1;
    header[3]=header[3]/gen(nc);
    return makevecteur(header,edata);
}
static const char _stereo2mono_s []="stereo2mono";
static define_unary_function_eval (__stereo2mono,&_stereo2mono,_stereo2mono_s);
define_unary_function_ptr5(at_stereo2mono,alias_at_stereo2mono,&__stereo2mono,0,true)

gen _plotspectrum(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT)
        return gentypeerr(contextptr);
    int nc,bd,sr,len,lfreq,ufreq;
    if (is_sound_data(g,nc,bd,sr,len)) {
        gen intrv=symbolic(at_interval,makesequence(0,sr/2));
        return _plotspectrum(makesequence(g,symbolic(at_equal,makesequence(at_range,intrv))),contextptr);
    } else if (g.subtype==_SEQ__VECT) {
        vecteur &gv=*g._VECTptr;
        if (gv.size()!=2 || !is_sound_data(gv.front(),nc,bd,sr,len))
            return gensizeerr(contextptr);
        vecteur data;
        if (nc>1)
            data=decode_chdata(*_stereo2mono(gv.front(),contextptr)._VECTptr->at(1)._VECTptr,bd);
        else data=decode_chdata(*gv.front()._VECTptr->at(1)._VECTptr,bd);
        len=data.size();
        if (!gv.back().is_symb_of_sommet(at_equal))
            return gensizeerr(contextptr);
        gen &lh=gv.back()._SYMBptr->feuille._VECTptr->front();
        gen &rh=gv.back()._SYMBptr->feuille._VECTptr->back();
        if (lh==at_range) {
            gen a,b;
            if (rh.type==_VECT) {
                if (rh._VECTptr->size()!=2 || !is_integer_vecteur(*rh._VECTptr))
                    return gensizeerr(contextptr);
                a=rh._VECTptr->front();
                b=rh._VECTptr->back();
            } else if (rh.is_symb_of_sommet(at_interval)) {
                a=rh._SYMBptr->feuille._VECTptr->front();
                b=rh._SYMBptr->feuille._VECTptr->back();
            } else return gensizeerr(contextptr);
            if (!is_integer(a) || !is_integer(b))
                return gensizeerr(contextptr);
            lfreq=std::max(0,a.val);
            ufreq=std::min(sr/2,b.val);
            if (lfreq>=ufreq)
                return gensizeerr(contextptr);
        }
        int n=nextpow2(len);
        data.resize(n,0);
        vecteur spec=*_fft(data,contextptr)._VECTptr;
        vecteur nodes;
        int dfreq=ufreq-lfreq,n1=((long)n*(long)lfreq)/(long)sr,n2=((long)n*(long)ufreq)/(long)sr;
        int width=std::min(dfreq,std::max(1500,dfreq/5));
        nodes.reserve(std::floor(width));
        int step=(n2-n1)/(2*width);
        if (step==0) step=1;
        double f;
        for (int i=n1;i<n2;++i) {
            f=std::max(f,_evalf(pow(_abs(spec[i],contextptr),2)/gen(n),contextptr).DOUBLE_val());
            if (i%step==0) {
                nodes.push_back(makecomplex(((long)i*(long)sr)/(long)n,f));
                f=0;
            }
        }
        nodes.push_back(ufreq+1);
        nodes.push_back(lfreq);
        nodes.push_back(symbolic(at_equal,makesequence(at_display,_FILL_POLYGON)));
        return _polygone(change_subtype(nodes,_SEQ__VECT),contextptr);
    } else return gentypeerr(contextptr);
}
static const char _plotspectrum_s []="plotspectrum";
static define_unary_function_eval (__plotspectrum,&_plotspectrum,_plotspectrum_s);
define_unary_function_ptr5(at_plotspectrum,alias_at_plotspectrum,&__plotspectrum,0,true)

gen _channels(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    int nc,bd,sr,len;
    if (!is_sound_data(g,nc,bd,sr,len))
        return gentypeerr(contextptr);
    return nc;
}
static const char _channels_s []="channels";
static define_unary_function_eval (__channels,&_channels,_channels_s);
define_unary_function_ptr5(at_channels,alias_at_channels,&__channels,0,true)

gen _bit_depth(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    int nc,bd,sr,len;
    if (!is_sound_data(g,nc,bd,sr,len))
        return gentypeerr(contextptr);
    return bd;
}
static const char _bit_depth_s []="bit_depth";
static define_unary_function_eval (__bit_depth,&_bit_depth,_bit_depth_s);
define_unary_function_ptr5(at_bit_depth,alias_at_bit_depth,&__bit_depth,0,true)

gen _samplerate(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    int nc,bd,sr,len;
    if (!is_sound_data(g,nc,bd,sr,len))
        return gentypeerr(contextptr);
    return sr;
}
static const char _samplerate_s []="samplerate";
static define_unary_function_eval (__samplerate,&_samplerate,_samplerate_s);
define_unary_function_ptr5(at_samplerate,alias_at_samplerate,&__samplerate,0,true)

gen _duration(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    int nc,bd,sr,len;
    if (!is_sound_data(g,nc,bd,sr,len))
        return gentypeerr(contextptr);
    return _evalf(fraction(len,sr),contextptr);
}
static const char _duration_s []="duration";
static define_unary_function_eval (__duration,&_duration,_duration_s);
define_unary_function_ptr5(at_duration,alias_at_duration,&__duration,0,true)

gen _channel_data(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT)
        return gentypeerr(contextptr);
    vecteur opts;
    if (g.subtype==_SEQ__VECT) {
        if (g._VECTptr->front().type!=_VECT)
            return gentypeerr(contextptr);
        opts=vecteur(g._VECTptr->begin()+1,g._VECTptr->end());
    }
    const gen &data=g.subtype==_SEQ__VECT?g._VECTptr->front():g;
    int nc,bd,sr,len;
    if (!is_sound_data(data,nc,bd,sr,len))
        return gentypeerr(contextptr);
    int chan=0,slice_start=0,slice_len=len;
    bool asmatrix=false;
    for (const_iterateur it=opts.begin();it!=opts.end();++it) {
        if (it->is_integer()) {
            if (it->val<1 || it->val>nc)
                return gensizeerr(contextptr);
            chan=it->val;
        } else if (*it==at_matrix)
            asmatrix=true;
        else if (it->is_symb_of_sommet(at_equal)) {
            gen &lh=it->_SYMBptr->feuille._VECTptr->front();
            gen &rh=it->_SYMBptr->feuille._VECTptr->back();
            if (lh==at_range) {
                if (rh.type==_VECT) {
                    if (rh._VECTptr->size()!=2)
                        return gendimerr(contextptr);
                    if (!is_integer_vecteur(*rh._VECTptr))
                        return gensizeerr(contextptr);
                    int start=rh._VECTptr->front().val,stop=rh._VECTptr->back().val;
                    slice_start=start-1; slice_len=stop-start+1;
                } else if (rh.is_symb_of_sommet(at_interval)) {
                    gen &a=rh._SYMBptr->feuille._VECTptr->front();
                    gen &b=rh._SYMBptr->feuille._VECTptr->back();
                    if (!a.is_integer() || !b.is_integer()) {
                        if (!is_real(a,contextptr) || !is_real(b,contextptr))
                            return gensizeerr(contextptr);
                        slice_start=std::floor(sr*_evalf(a,contextptr).DOUBLE_val());
                        slice_len=std::floor(sr*_evalf(b-a,contextptr).DOUBLE_val());
                    } else {
                        slice_start=a.val-1;
                        slice_len=(b-a).val+1;
                    }
                } else return gensizeerr(contextptr);
            }
        }
    }
    if (slice_start<0 || slice_start>=len || slice_len<0 || slice_len>len || slice_start+slice_len>len)
        return gensizeerr(contextptr);
    if (chan==0) {
        vecteur ret;
        for (const_iterateur it=data._VECTptr->begin()+1;it!=data._VECTptr->end();++it) {
            ret.push_back(decode_chdata(*it->_VECTptr,bd,slice_start,slice_len));
        }
        if (ret.size()==1)
            return ret.front();
        return asmatrix?ret:change_subtype(ret,_SEQ__VECT);
    }
    return decode_chdata(*data._VECTptr->at(chan)._VECTptr,bd,slice_start,slice_len);
}
static const char _channel_data_s []="channel_data";
static define_unary_function_eval (__channel_data,&_channel_data,_channel_data_s);
define_unary_function_ptr5(at_channel_data,alias_at_channel_data,&__channel_data,0,true)

gen _cross_correlation(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT || g.subtype!=_SEQ__VECT)
        return gentypeerr(contextptr);
    vecteur &args=*g._VECTptr;
    if (args.size()!=2 || args.front().type!=_VECT || args.back().type!=_VECT)
        return gensizeerr(contextptr);
    vecteur A=*args.front()._VECTptr,B=*args.back()._VECTptr;
    int m=A.size(),n=B.size(),N=nextpow2(std::max(n,m));
    A.resize(2*N,0);
    B.resize(2*N,0);
    vecteur a=*_fft(A,contextptr)._VECTptr,b=*_fft(B,contextptr)._VECTptr;
    vecteur cc_ffted=*_pointprod(makesequence(a,conj(b,contextptr)),contextptr)._VECTptr;
    vecteur cc=*_apply(makesequence(at_real,_ifft(cc_ffted,contextptr)),contextptr)._VECTptr;
    reverse(cc.begin(),cc.begin()+N);
    reverse(cc.begin()+N,cc.end());
    return vecteur(cc.begin()+N-m,cc.end()-N+n-1);
}
static const char _cross_correlation_s []="cross_correlation";
static define_unary_function_eval (__cross_correlation,&_cross_correlation,_cross_correlation_s);
define_unary_function_ptr5(at_cross_correlation,alias_at_cross_correlation,&__cross_correlation,0,true)

gen _auto_correlation(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG) {
        if (g.subtype==-1) return g;
        return _auto_correlation(_readwav(g,contextptr),contextptr);
    }
    return _cross_correlation(makesequence(g,g),contextptr);
}
static const char _auto_correlation_s []="auto_correlation";
static define_unary_function_eval (__auto_correlation,&_auto_correlation,_auto_correlation_s);
define_unary_function_ptr5(at_auto_correlation,alias_at_auto_correlation,&__auto_correlation,0,true)

void lowpass(vecteur &data,double cutoff,int sr) {
    double rc=0.15915494309/cutoff;
    double dt=1.0/sr;
    gen s=data.front(),alpha(dt/(rc+dt));
    for (iterateur it=data.begin();it!=data.end();++it) {
        s+=alpha*(*it-s);
        *it=s;
    }
}

void highpass(vecteur &data,double cutoff,int sr) {
    double rc=0.15915494309/cutoff;
    double dt=1.0/sr;
    gen alpha(rc/(rc+dt)),prevdata=data.front(),temp;
    for (iterateur it=data.begin()+1;it!=data.end();++it) {
        temp=*it;
        *it=alpha*(*(it-1)+*it-prevdata);
        prevdata=temp;
    }
}

gen filter(const vecteur &args,filter_type typ,GIAC_CONTEXT) {
    double cutoff=_evalf(args.at(1),contextptr).DOUBLE_val();
    int nc,bd,sr,len;
    if (is_sound_data(*args.front()._VECTptr,nc,bd,sr,len)) {
        if (cutoff<=0 || cutoff>=sr/2)
            return gensizeerr(contextptr);
        gen opt(undef);
        if (args.size()>2) {
            if (!args[2].is_symb_of_sommet(at_equal))
                return gensizeerr(contextptr);
            if (args[2]._SYMBptr->feuille._VECTptr->front()==at_normalize)
                opt=args[2];
        }
        matrice data;
        for (int i=1;i<=nc;++i) {
            vecteur chdata=decode_chdata(*args.front()._VECTptr->at(i)._VECTptr,bd);
            switch(typ) {
            case _LOWPASS_FILTER: lowpass(chdata,cutoff,sr); break;
            case _HIGHPASS_FILTER: highpass(chdata,cutoff,sr); break;
            }
            data.push_back(chdata);
        }
        gen argv=makesequence(symbolic(at_equal,makesequence(at_samplerate,sr)),
                              symbolic(at_equal,makesequence(at_bit_depth,bd)),data);
        if (!is_undef(opt)) argv._VECTptr->push_back(opt);
        return _createwav(argv,contextptr);
    } else {
        vecteur data=*args.front()._VECTptr;
        sr=44100;
        if (args.size()>2) {
            if (!is_integer(args.at(2)) || (sr=args.at(2).val)<=0)
                return gentypeerr(contextptr);
        }
        if (cutoff<=0 || cutoff>=sr/2)
            return gensizeerr(contextptr);
        switch(typ) {
        case _LOWPASS_FILTER: lowpass(data,cutoff,sr); break;
        case _HIGHPASS_FILTER: highpass(data,cutoff,sr); break;
        }
        return data;
    }
}

gen _lowpass(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT || g.subtype!=_SEQ__VECT || g._VECTptr->size()<2 ||
            g._VECTptr->front().type!=_VECT || !is_real(g._VECTptr->at(1),contextptr))
        return gentypeerr(contextptr);
    return filter(*g._VECTptr,_LOWPASS_FILTER,contextptr);
}
static const char _lowpass_s []="lowpass";
static define_unary_function_eval (__lowpass,&_lowpass,_lowpass_s);
define_unary_function_ptr5(at_lowpass,alias_at_lowpass,&__lowpass,0,true)

gen _highpass(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT || g.subtype!=_SEQ__VECT || g._VECTptr->size()<2 ||
            g._VECTptr->front().type!=_VECT || !is_real(g._VECTptr->at(1),contextptr))
        return gentypeerr(contextptr);
    return filter(*g._VECTptr,_HIGHPASS_FILTER,contextptr);
}
static const char _highpass_s []="highpass";
static define_unary_function_eval (__highpass,&_highpass,_highpass_s);
define_unary_function_ptr5(at_highpass,alias_at_highpass,&__highpass,0,true)

gen _moving_average(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT || g.subtype!=_SEQ__VECT)
        return gentypeerr(contextptr);
    vecteur &gv=*g._VECTptr;
    if (gv.size()!=2)
        return gensizeerr("Wrong number of input arguments");
    if (gv.front().type!=_VECT)
        return gensizeerr("First argument must be an array");
    if (!gv.back().is_integer() || gv.back().val<=0)
        return gensizeerr("Second argument must be a positive integer");
    vecteur &s=*gv.front()._VECTptr;
    int n=gv.back().val,len=s.size();
    if (n>len)
        return gensizeerr("Filter length exceeds array size");
    vecteur res(len-n+1);
    gen acc(0);
    for (int i=0;i<n;++i) acc+=s[i];
    res[0]=acc;
    for (int i=n;i<len;++i) {
        acc-=s[i-n];
        acc+=s[i];
        res[i-n+1]=acc;
    }
    return multvecteur(fraction(1,n),res);
}
static const char _moving_average_s []="moving_average";
static define_unary_function_eval (__moving_average,&_moving_average,_moving_average_s);
define_unary_function_ptr5(at_moving_average,alias_at_moving_average,&__moving_average,0,true)

gen _resample(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
#ifndef HAVE_LIBSAMPLERATE
    *logptr(contextptr) << "Error: libsamplerate is required for resampling audio\n";
    return vecteur(0);
#else
    if (g.type!=_VECT)
        return gentypeerr(contextptr);
    int nc,bd,sr,len;
    const gen &snd=g.subtype==_SEQ__VECT?g._VECTptr->front():g;
    if (!is_sound_data(snd,nc,bd,sr,len))
        return gentypeerr(contextptr);
    int nsr=44100;
    int quality=2;
    if (g.subtype==_SEQ__VECT) {
        vecteur &gv=*g._VECTptr;
        if (gv.size()<2 || !gv[1].is_integer() || (nsr=gv[1].val)<1)
            return gensizeerr(contextptr);
        if (gv.size()>2) {
            if (!gv[2].is_integer() || (quality=gv[2].val)<0 || (quality>4))
                return gensizeerr(contextptr);
        }
    }
    matrice chdata;
    for (int i=1;i<=nc;++i) {
        vecteur ch=decode_chdata(*snd._VECTptr->at(i)._VECTptr,bd);
        chdata.push_back(ch);
    }
    int nlen=((long)len*(long)nsr)/(long)sr;
    SRC_DATA data;
    data.input_frames=len;
    data.output_frames=nlen;
    float *indata=new float[len*nc];
    data.data_out=new float[nlen*nc];
    data.src_ratio=double(nsr)/double(sr);
    for (int i=0;i<len;++i) {
        for (int j=0;j<nc;++j) {
            indata[i*nc+j]=_evalf(chdata[j][i],contextptr).DOUBLE_val();
        }
    }
    data.data_in=indata;
    switch(quality) {
    case 0:
        quality=SRC_LINEAR;
        break;
    case 1:
        quality=SRC_ZERO_ORDER_HOLD;
        break;
    case 2:
        quality=SRC_SINC_FASTEST;
        break;
    case 3:
        quality=SRC_SINC_MEDIUM_QUALITY;
        break;
    case 4:
        quality=SRC_SINC_BEST_QUALITY;
        break;
    default:
        assert(false);
    }
    src_simple(&data,quality,nc);
    matrice output=*_matrix(makesequence(nc,nlen,0),contextptr)._VECTptr;
    for (int i=0;i<nlen;++i) {
        for (int j=0;j<nc;++j) {
            output[j]._VECTptr->at(i)=data.data_out[i*nc+j];
        }
    }
    delete[] indata;
    delete[] data.data_out;
    vecteur header=*snd._VECTptr->front()._VECTptr;
    header[2]=nsr;
    header[3]=(bd*nc*nlen)/8;
    vecteur ret;
    ret.push_back(header);
    for (const_iterateur it=output.begin();it!=output.end();++it) {
        ret.push_back(encode_chdata(*it->_VECTptr,bd,1.0,contextptr));
    }
    return ret;
#endif
}
static const char _resample_s []="resample";
static define_unary_function_eval (__resample,&_resample,_resample_s);
define_unary_function_ptr5(at_resample,alias_at_resample,&__resample,0,true)

int varcount=0;

gen _convolution(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT || g.subtype!=_SEQ__VECT)
        return gentypeerr(contextptr);
    const vecteur &args=*g._VECTptr;
    if (!args.empty() && args.front().type!=_VECT) {
        // convolve real functions
        gen T(0),var=identificateur("x"),tvar=identificateur(" tau"+print_INT_(++varcount));
        int n=args.size(),optstart=2;
        if (n<2) return gensizeerr(contextptr);
        const gen &f1=args[0],&f2=args[1];
        if (n>2) {
            if (args[optstart].type==_IDNT)
                var=args[optstart++];
            // parse options
            for (const_iterateur it=args.begin()+optstart;it!=args.end();++it) {
                if (!it->is_symb_of_sommet(at_equal))
                    return gensizeerr(contextptr);
                vecteur &s=*it->_SYMBptr->feuille._VECTptr;
                if (s.front()==at_shift)
                    T=s.back();
                else return gensizeerr(contextptr);
            }
        }
        giac_assume(symb_superieur_egal(tvar,0),contextptr);
        gen minf=symbolic(at_neg,unsigned_inf),pinf=symbolic(at_plus,unsigned_inf),c;
        c=_integrate(makesequence(f1*_Heaviside(var,contextptr)*
                                  subst(f2*_Heaviside(var,contextptr),var,tvar-var,false,contextptr),
                                  var,minf,pinf),contextptr);
        _purge(tvar,contextptr);
        if (is_one(_contains(makesequence(_lname(c,contextptr),var),contextptr)))
            return gensizeerr("failed to integrate");
        c=subst(c,tvar,var-T,false,contextptr)*_Heaviside(var-T,contextptr);
        return c;
    }
    // convolve sequences
    if (args.size()!=2 || args.front().type!=_VECT || args.back().type!=_VECT)
        return gensizeerr(contextptr);
    vecteur A=*args.front()._VECTptr,B=*args.back()._VECTptr;
    int lenA=A.size(),lenB=B.size(),len=2*nextpow2(std::max(lenA,lenB));
    A.resize(len-1,0);
    B.resize(len-1,0);
    vecteur a=*_fft(A,contextptr)._VECTptr,b=*_fft(B,contextptr)._VECTptr;
    vecteur cv=*_apply(makesequence(at_real,_ifft(_pointprod(makesequence(a,b),
                                                             contextptr),
                                                  contextptr)),
                       contextptr)._VECTptr;
    cv.resize(lenA+lenB-1);
    return cv;
}
static const char _convolution_s []="convolution";
static define_unary_function_eval (__convolution,&_convolution,_convolution_s);
define_unary_function_ptr5(at_convolution,alias_at_convolution,&__convolution,0,true)

gen _boxcar(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT || g.subtype!=_SEQ__VECT || g._VECTptr->size()!=3)
        return gentypeerr(contextptr);
    const vecteur &args=*g._VECTptr;
    return _Heaviside(args[2]-args[0],contextptr)-_Heaviside(args[2]-args[1],contextptr);
}
static const char _boxcar_s []="boxcar";
static define_unary_function_eval (__boxcar,&_boxcar,_boxcar_s);
define_unary_function_ptr5(at_boxcar,alias_at_boxcar,&__boxcar,0,true)

gen _rect(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    gen hf=fraction(1,2);
    return _boxcar(makesequence(-hf,hf,g),contextptr);
}
static const char _rect_s []="rect";
static define_unary_function_eval (__rect,&_rect,_rect_s);
define_unary_function_ptr5(at_rect,alias_at_rect,&__rect,0,true)

gen _tri(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    return (1-g)*(_Heaviside(g,contextptr)-_Heaviside(g-1,contextptr))+
           (1+g)*(_Heaviside(-g,contextptr)-_Heaviside(-g-1,contextptr));
}
static const char _tri_s []="tri";
static define_unary_function_eval (__tri,&_tri,_tri_s);
define_unary_function_ptr5(at_tri,alias_at_tri,&__tri,0,true)

gen _sinc(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (is_zero(g))
        return gen(1);
    return sin(g,contextptr)/g;
}
static const char _sinc_s []="sinc";
static define_unary_function_eval (__sinc,&_sinc,_sinc_s);
define_unary_function_ptr5(at_sinc,alias_at_sinc,&__sinc,0,true)

#define MAX_TAILLE 500

/* return true iff g is significantly simpler than h */
bool is_simpler(const gen &g,const gen &h,double scale=1.0) {
    return scale*taille(h,MAX_TAILLE)>taille(g,MAX_TAILLE);
}

bool is_rational_wrt(const gen &e,const identificateur &x) {
    return rlvarx(e,x).size()<=1;
}

bool is_const_wrt_x(const gen &g,const identificateur &x,GIAC_CONTEXT) {
    return is_zero(_contains(makesequence(_lname(g,contextptr),x),contextptr));
}

gen factorise(const gen &g,GIAC_CONTEXT) {
    return factorcollect(g,false,contextptr);
}

bool ispoly(const gen &e,const identificateur &x,gen &d,GIAC_CONTEXT) {
    if (is_const_wrt_x(e,x,contextptr)) {
        d=gen(0);
        return true;
    }
    if (!is_rational_wrt(e,x) || !is_constant_wrt(_denom(e,contextptr),x,contextptr))
        return false;
    d=_degree(makesequence(e,x),contextptr);
    return d.is_integer() && !is_zero(d);
}

void constlin_terms(const gen &g,const identificateur &x,gen &lt,gen &c,gen &rest,GIAC_CONTEXT) {
    gen e=expand(g,contextptr);
    vecteur terms=(e.is_symb_of_sommet(at_plus) && e._SYMBptr->feuille.type==_VECT?
                   *e._SYMBptr->feuille._VECTptr:vecteur(1,e));
    gen a,b;
    rest=c=lt=gen(0);
    for (const_iterateur it=terms.begin();it!=terms.end();++it) {
        if (is_const_wrt_x(*it,x,contextptr))
            c+=*it;
        else if (is_linear_wrt(*it,x,a,b,contextptr)) {
            lt+=a;
            c+=b;
        } else rest+=*it;
    }
}

gen logabs_expand(const gen &g,const identificateur &x,GIAC_CONTEXT) {
    if (g.type==_VECT) {
        vecteur res;
        for (const_iterateur it=g._VECTptr->begin();it!=g._VECTptr->end();++it) {
            res.push_back(logabs_expand(*it,x,contextptr));
        }
        return gen(res,g.subtype);
    }
    gen a,b;
    if (g.is_symb_of_sommet(at_ln) && g._SYMBptr->feuille.is_symb_of_sommet(at_abs) &&
            is_linear_wrt(g._SYMBptr->feuille._SYMBptr->feuille,x,a,b,contextptr) &&
            !is_zero(a) && !is_one(a) && is_zero(im(a,contextptr)) && is_zero(im(b,contextptr)))
        return ln(_abs(x+b/a,contextptr),contextptr)+ln(_abs(a,contextptr),contextptr);
    if (g.type==_SYMB)
        return symbolic(g._SYMBptr->sommet,logabs_expand(g._SYMBptr->feuille,x,contextptr));
    return g;
}

vecteur analyze_terms(const gen &g,const identificateur &x,GIAC_CONTEXT) {
    vecteur terms,ret;
    if (g.is_symb_of_sommet(at_plus)) {
        if (g._SYMBptr->feuille.type==_VECT)
            terms=*g._SYMBptr->feuille._VECTptr;
        else terms=vecteur(1,g._SYMBptr->feuille);
    } else terms=vecteur(1,g);
    for (const_iterateur it=terms.begin();it!=terms.end();++it) {
        vecteur factors;
        gen cnst(1),rest(1),deg(0),exprest(0),sh(0),rt=ratnormal(*it,contextptr),st;
        if (!is_const_wrt_x(rt,x,contextptr))
            rt=factorise(rt,contextptr);
        if (rt.is_symb_of_sommet(at_neg)) {
            cnst=gen(-1);
            rt=rt._SYMBptr->feuille;
        }
        if (rt.is_symb_of_sommet(at_prod) && rt._SYMBptr->feuille.type==_VECT)
            factors=*rt._SYMBptr->feuille._VECTptr;
        else factors=vecteur(1,rt);
        for (int i=factors.size();i-->0;) {
            gen &fac=factors[i];
            if (fac.is_symb_of_sommet(at_inv)) {
                if (fac._SYMBptr->feuille.is_symb_of_sommet(at_prod) &&
                    fac._SYMBptr->feuille._SYMBptr->feuille.type==_VECT) {
                    const vecteur &d=*fac._SYMBptr->feuille._SYMBptr->feuille._VECTptr;
                    for (const_iterateur jt=d.begin();jt!=d.end();++jt) {
                        factors.push_back(symbolic(at_inv,*jt));
                    }
                    factors.erase(factors.begin()+i);
                }
            }
        }
        for (const_iterateur jt=factors.begin();jt!=factors.end();++jt) {
            if (jt->is_symb_of_sommet(at_exp)) {
                gen lt1,lt2,c1,c2,rest1,rest2;
                constlin_terms(re(jt->_SYMBptr->feuille,contextptr),x,lt1,c1,rest1,contextptr);
                constlin_terms(im(jt->_SYMBptr->feuille,contextptr),x,lt2,c2,rest2,contextptr);
                cnst=cnst*exp(c1+cst_i*c2,contextptr);
                sh+=lt2;
                exprest+=rest1+x*lt1+cst_i*rest2;
            } else if (is_const_wrt_x(*jt,x,contextptr))
                cnst=cnst*(*jt);
            else if (ispoly(*jt,x,st,contextptr)) {
                deg+=st;
                cnst=cnst*_lcoeff(makesequence(*jt,x),contextptr);
            } else rest=rest*(*jt);
        }
        rest=rest*exp(exprest,contextptr);
        cnst=recursive_normal(_lin(cnst,contextptr),contextptr);
        ret.push_back(makevecteur(cnst,sh,deg,rest));
    }
    return ret;
}

bool has_rootof(const gen &g) {
    if (g.is_symb_of_sommet(at_rootof)) return true;
    if (g.type==_VECT) {
        for (const_iterateur it=g._VECTptr->begin();it!=g._VECTptr->end();++it) {
            if (has_rootof(*it)) return true;
        }
    } else if (g.type==_SYMB) return has_rootof(g._SYMBptr->feuille);
    return false;
}

bool has_partial_diff(const gen &g) {
    if (g.is_symb_of_sommet(at_of) &&
            g._SYMBptr->feuille._VECTptr->front().is_symb_of_sommet(at_derive))
        return true;
    if (g.type==_VECT) {
        for (const_iterateur it=g._VECTptr->begin();it!=g._VECTptr->end();++it) {
            if (has_partial_diff(*it)) return true;
        }
    } else if (g.type==_SYMB) return has_partial_diff(g._SYMBptr->feuille);
    return false;
}

bool is_heavisided(const gen &g,const identificateur &x,gen &rest,gen &a,gen &b,GIAC_CONTEXT) {
    if (g.is_symb_of_sommet(at_neg)) {
        if (!is_heavisided(g._SYMBptr->feuille,x,rest,a,b,contextptr))
            return false;
        rest=-rest;
        return true;
    }
    if (!g.is_symb_of_sommet(at_prod) || g._SYMBptr->feuille.type!=_VECT)
        return false;
    rest=gen(1);
    const vecteur &factors=*g._SYMBptr->feuille._VECTptr;
    bool yes=false;
    for (const_iterateur it=factors.begin();it!=factors.end();++it) {
        if (!yes && it->is_symb_of_sommet(at_Heaviside) &&
                is_linear_wrt(it->_SYMBptr->feuille,x,a,b,contextptr) && !is_zero(a))
            yes=true;
        else rest=rest*(*it);
    }
    return yes;
}

gen lin_cplx(const gen &g_orig,GIAC_CONTEXT) {
    gen g=_lin(g_orig,contextptr),ret(1);
    if (g.is_symb_of_sommet(at_neg)) {
        ret=gen(-1);
        g=g._SYMBptr->feuille;
    }
    vecteur factors;
    if (g.is_symb_of_sommet(at_prod) && g._SYMBptr->feuille.type==_VECT)
        factors=*g._SYMBptr->feuille._VECTptr;
    else factors=vecteur(1,g);
    for (const_iterateur it=factors.begin();it!=factors.end();++it) {
        if (it->is_symb_of_sommet(at_exp)) {
            gen ex=it->_SYMBptr->feuille;
            ret=ret*exp(cst_i*im(ex,contextptr),contextptr)*exp(re(ex,contextptr),contextptr);
        } else ret=ret*(*it);
    }
    return ret;
}

gen hcollect(const gen &g,const gen &h,const identificateur &x,gen &cnst,GIAC_CONTEXT) {
    gen f=factorise(lin_cplx(g,contextptr)+lin_cplx(h,contextptr),contextptr);
    gen rest(1),r1,r2,a1,a2,b1,b2,hvs(undef);
    vecteur factors;
    cnst=gen(1);
    if (f.is_symb_of_sommet(at_neg)) {
        rest=gen(-1);
        f=f._SYMBptr->feuille;
    }
    if (f.is_symb_of_sommet(at_prod) && f._SYMBptr->feuille.type==_VECT)
        factors=*f._SYMBptr->feuille._VECTptr;
    else factors=vecteur(1,f);
    for (const_iterateur it=factors.begin();it!=factors.end();++it) {
        if (is_undef(hvs) && it->is_symb_of_sommet(at_plus) && it->_SYMBptr->feuille.type==_VECT &&
                it->_SYMBptr->feuille._VECTptr->size()==2 &&
                is_heavisided(it->_SYMBptr->feuille._VECTptr->front(),x,r1,a1,b1,contextptr) &&
                is_heavisided(it->_SYMBptr->feuille._VECTptr->back(),x,r2,a2,b2,contextptr) &&
                is_zero(ratnormal(b1*a2-b2*a1,contextptr)) && is_strictly_positive(-a1*a2,contextptr) &&
                is_zero(recursive_normal(subst(r1,x,-b1/a1+_sign(a1,contextptr)*x,false,contextptr)-
                                         subst(r2,x,-b2/a2+_sign(a2,contextptr)*x,false,contextptr),
                                         contextptr))) {
            hvs=subst(r1,x,-b1/a1+_sign(a1,contextptr)*_abs(x+b1/a1,contextptr),false,contextptr);
        } else if (is_const_wrt_x(*it,x,contextptr))
            cnst=cnst*(*it);
        else rest=rest*(*it);
    }
    return rest*hvs;
}

gen Heaviside2abs(const gen &g,const identificateur &x,GIAC_CONTEXT) {
    gen h=expand(g,contextptr),e,c;
    vecteur terms;
    if (h.is_symb_of_sommet(at_plus) && h._SYMBptr->feuille.type==_VECT)
        terms=*h._SYMBptr->feuille._VECTptr;
    else terms=vecteur(1,h);
    for (int i=terms.size();i-->0;) {
        for (int j=terms.size();j-->i+1;) {
            if (!is_undef(e=hcollect(terms[i],terms[j],x,c,contextptr))) {
                terms.erase(terms.begin()+j);
                terms.erase(terms.begin()+i);
                terms.push_back(c*e);
                break;
            }
        }
    }
    return symbolic(at_plus,gen(terms,_SEQ__VECT));
}

bool has_integral(const gen &g) {
    if (g.type==_VECT) {
        for (const_iterateur it=g._VECTptr->begin();it!=g._VECTptr->end();++it) {
            if (has_integral(*it))
                return true;
        }
        return false;
    }
    if (g.type==_SYMB)
        return has_integral(g._SYMBptr->feuille);
    if (g.is_symb_of_sommet(at_integrate) || g.is_symb_of_sommet(at_int) ||
            g.is_symb_of_sommet(at_Int))
        return true;
    return false;
}

bool is_same(const gen &g,const gen &h,GIAC_CONTEXT) {
    return is_zero(ratnormal(g-h,contextptr));
}

vecteur fourier_terms(const gen &g_orig,const identificateur &x,bool do_simp,GIAC_CONTEXT) {
    gen g;
    if (do_simp)
        g=expand(ratnormal(logabs_expand(lnexpand(_lin(_trig2exp(_hyp2exp(_pow2exp(g_orig,contextptr),
                 contextptr),contextptr),contextptr),contextptr),x,contextptr),contextptr),contextptr);
    else g=g_orig;
    vecteur terms=analyze_terms(g,x,contextptr);
    // decompose terms into sums of partial fractions
    for (int i=terms.size();i-->0;) {
        assert(terms[i].type==_VECT);
        const vecteur &v=*terms[i]._VECTptr;
        if (!is_rational_wrt(v[3],x) ||
            is_constant_wrt(_denom(v[3],contextptr),x,contextptr)) continue;
        vecteur rterms=analyze_terms(_cpartfrac(makesequence(v[3],x),contextptr),x,contextptr);
        for (const_iterateur it=rterms.begin();it!=rterms.end();++it) {
            assert(it->type==_VECT);
            const vecteur &w=*(it->_VECTptr);
            terms.push_back(makevecteur(v[0]*w[0],v[1],v[2]+w[2],expand(w[3],contextptr)));
        }
        terms.erase(terms.begin()+i);
    }
    // collect terms to reduce complexity
    for (int i=0;i<int(terms.size());++i) {
        vecteur &v=*terms[i]._VECTptr;
        for (int j=terms.size();j-->i+1;) {
            const vecteur &w=*terms[j]._VECTptr;
            if (is_same(v[3],w[3],contextptr) && is_same(v[1],w[1],contextptr) &&
                    is_same(v[2],w[2],contextptr)) {
                v[0]+=w[0];
                terms.erase(terms.begin()+j);
            }
        }
    }
    // convert: f(-x)*Heaviside(x)+f(x)*Heaviside(-x) -> f(-abs(x))
    for (int i=terms.size();i-->0;) {
        vecteur &v=*terms[i]._VECTptr;
        for (int j=terms.size();j-->i+1;) {
            const vecteur &w=*terms[j]._VECTptr;
            gen e,c;
            if (is_same(v[1],w[1],contextptr) && is_zero(v[2]-w[2]) &&
                    !is_undef(e=hcollect(v[3]*v[0],w[3]*w[0],x,c,contextptr))) {
                e=makevecteur(c,v[1],v[2],e);
                terms.erase(terms.begin()+j);
                terms.erase(terms.begin()+i);
                terms.push_back(e);
                break;
            }
        }
    }
    return terms;
}

enum FourierFuncType {
    _FOURIER_FUNCTYPE_UNKNOWN=0,
    _FOURIER_FUNCTYPE_ONE=1,
    _FOURIER_FUNCTYPE_DIRAC=2,
    _FOURIER_FUNCTYPE_INV_MONOM=3,
    _FOURIER_FUNCTYPE_GAUSSIAN=4,
    _FOURIER_FUNCTYPE_LOGABSX=5,
    _FOURIER_FUNCTYPE_BESSELJ=6,
    _FOURIER_FUNCTYPE_SGN=7,
    _FOURIER_FUNCTYPE_ABSX_ALPHA=8,
    _FOURIER_FUNCTYPE_INVABSX=9,
    _FOURIER_FUNCTYPE_GAMMA=10,
    _FOURIER_FUNCTYPE_EXPEXP=11,
    _FOURIER_FUNCTYPE_AIRY_AI=12,
    _FOURIER_FUNCTYPE_EXP_HEAVISIDE=13,
    _FOURIER_FUNCTYPE_HEAVISIDE=14,
    _FOURIER_FUNCTYPE_ATAN_OVERX=15,
    _FOURIER_FUNCTYPE_EXPABSX=16,
    _FOURIER_FUNCTYPE_EXPABSX_OVERX=17,
    _FOURIER_FUNCTYPE_COSECH=18,
    _FOURIER_FUNCTYPE_SECH=19,
    _FOURIER_FUNCTYPE_SECH_2=20,
    _FOURIER_FUNCTYPE_CONVOLUTION=21,
    _FOURIER_FUNCTYPE_PIECEWISE=22,
    _FOURIER_FUNCTYPE_FUNC=23,
    _FOURIER_FUNCTYPE_DIFF=24,
    _FOURIER_FUNCTYPE_PARTIAL_DIFF=25
};

static vecteur fourier_table;
static vecteur laplace_table;

gen _addtable(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT || g.subtype!=_SEQ__VECT)
        return gentypeerr(contextptr);
    const vecteur &args=*g._VECTptr;
    if (args.size()!=5)
        return gensizeerr(contextptr);
    int ti=0;
    if (args[0]==at_fourier) ti=1;
    if (args[0]==at_laplace) ti=2;
    if (!args[1].is_symb_of_sommet(at_of) || !args[2].is_symb_of_sommet(at_of) ||
            args[1]._SYMBptr->feuille._VECTptr->front().type!=_IDNT ||
            args[2]._SYMBptr->feuille._VECTptr->front().type!=_IDNT ||
            args[3].type!=_IDNT || args[4].type!=_IDNT)
        return gensizeerr(contextptr);
    const identificateur &f=*args[1]._SYMBptr->feuille._VECTptr->front()._IDNTptr;
    const identificateur &F=*args[2]._SYMBptr->feuille._VECTptr->front()._IDNTptr;
    const identificateur &t=*args[3]._IDNTptr,&s=*args[4]._IDNTptr;
    const vecteur fvars=(args[1]._SYMBptr->feuille._VECTptr->back().type==_VECT?
                             *args[1]._SYMBptr->feuille._VECTptr->back()._VECTptr:
                             vecteur(1,args[1]._SYMBptr->feuille._VECTptr->back()));
    const vecteur Fvars=(args[2]._SYMBptr->feuille._VECTptr->back().type==_VECT?
                             *args[2]._SYMBptr->feuille._VECTptr->back()._VECTptr:
                             vecteur(1,args[2]._SYMBptr->feuille._VECTptr->back()));
    if (fvars.size()!=Fvars.size()) return gensizeerr(contextptr);
    int k=-1;
    for (int i=fvars.size();i-->0;) {
        if (fvars[i].type!=_IDNT || Fvars[i].type!=_IDNT) return gensizeerr(contextptr);
        const identificateur &x1=*fvars[i]._IDNTptr,&x2=*Fvars[i]._IDNTptr;
        if (x1==x2) continue;
        if (k>=0 || !(x1==t) || !(x2==s)) return gensizeerr(contextptr);
        k=i;
    }
    if (k<0) return gensizeerr(contextptr);
    if (ti<1 || ti>2) return gen(0);
    vecteur &tbl=(ti==1?fourier_table:laplace_table);
    for (const_iterateur it=tbl.begin();it!=tbl.end();++it) {
        const vecteur &v=*(it->_VECTptr);
        if (v[0]==f || v[0]==F || v[1]==f || v[1]==F) return gen(0);
    }
    tbl.push_back(makevecteur(f,F,fvars.size(),k));
    return gen(1);
}
static const char _addtable_s []="addtable";
static define_unary_function_eval (__addtable,&_addtable,_addtable_s);
define_unary_function_ptr5(at_addtable,alias_at_addtable,&__addtable,0,true)

vecteur fourier_table_find(const identificateur &f,int sz) {
    for (const_iterateur it=fourier_table.begin();it!=fourier_table.end();++it) {
        const vecteur &r=*(it->_VECTptr);
        if (r[2].val!=sz) continue;
        if (r[0]==f) return makevecteur(1,r[1],r[3]);
        if (r[1]==f) return makevecteur(2*cst_pi,r[0],r[3]);
    }
    return vecteur(0);
}

bool is_func(const gen &g,identificateur &f,vecteur &vars) {
    if (!g.is_symb_of_sommet(at_of) || g._SYMBptr->feuille._VECTptr->front().type!=_IDNT)
        return false;
    f=*g._SYMBptr->feuille._VECTptr->front()._IDNTptr;
    vars=(g._SYMBptr->feuille._VECTptr->back().type==_VECT?
              *g._SYMBptr->feuille._VECTptr->back()._VECTptr:
              vecteur(1,g._SYMBptr->feuille._VECTptr->back()));
    return true;
}

bool is_diff(const gen &g,const identificateur &x,gen &f,int &d) {
    if (!g.is_symb_of_sommet(at_derive) || g._SYMBptr->feuille.type!=_VECT)
        return false;
    const vecteur &args=*g._SYMBptr->feuille._VECTptr;
    int n;
    if (args.size()==2 && args.back()==x) d=1;
    else if (args.size()==3 && args[1]==x && args[2].is_integer() && (n=args[2].val)>0) d=n;
    else return false;
    f=args.front();
    return true;
}

bool is_partialdiff(const gen &g,identificateur &f,vecteur &vars,vecteur &deg,GIAC_CONTEXT) {
    if (!g.is_symb_of_sommet(at_of)) return false;
    gen df=g._SYMBptr->feuille._VECTptr->front();
    const gen &v=g._SYMBptr->feuille._VECTptr->back();
    if (v.type!=_VECT) return false;
    vars=*v._VECTptr;
    int n,sz=vars.size();
    deg=vecteur(sz,0);
    while (df.is_symb_of_sommet(at_derive) && df._SYMBptr->feuille.type==_VECT) {
        const vecteur &args=*df._SYMBptr->feuille._VECTptr;
        if (args.size()!=2 || !args.back().is_integer() || (n=args.back().val)<0 || n>=sz)
            return false;
        deg[n]+=gen(1);
        df=args.front();
    }
    if (df.type!=_IDNT || is_zero__VECT(deg,contextptr)) return false;
    f=*df._IDNTptr;
    return true;
}

gen lin_Heaviside(const gen &g_orig,const identificateur &x,GIAC_CONTEXT) {
    gen g=expand(g_orig,contextptr),rest(1),ret(0),a,b;
    if (g.is_symb_of_sommet(at_plus) && g._SYMBptr->feuille.type==_VECT) {
        const vecteur &terms=*g._SYMBptr->feuille._VECTptr;
        for (const_iterateur it=terms.begin();it!=terms.end();++it) {
            ret+=lin_Heaviside(*it,x,contextptr);
        }
        return ret;
    }
    if (g.is_symb_of_sommet(at_neg)) {
        rest=gen(-1);
        g=g._SYMBptr->feuille;
    }
    vecteur factors;
    matrice L;
    if (g.is_symb_of_sommet(at_prod) && g._SYMBptr->feuille.type==_VECT)
        factors=*g._SYMBptr->feuille._VECTptr;
    else factors=vecteur(1,g);
    int i=0;
    for (const_iterateur it=factors.begin();it!=factors.end();++it) {
        if (i<2 && it->is_symb_of_sommet(at_Heaviside) &&
                is_linear_wrt(it->_SYMBptr->feuille,x,a,b,contextptr)) {
            L.push_back(makevecteur(_sign(a,contextptr),-b/_abs(a,contextptr)));
            ++i;
        } else rest=rest*(*it);
    }
    if (i<2) {
        if (i==1) rest=rest*_Heaviside(L[0][0]*x-L[0][1],contextptr);
        return rest;
    }
    if (is_one(L[0][0]*L[1][0])) {
        if (is_one(L[0][0]))
            rest=rest*_Heaviside(x-(is_greater(L[1][1],L[0][1],contextptr)?L[1][1]:L[0][1]),contextptr);
        else
            rest=rest*_Heaviside((is_greater(L[1][1],L[0][1],contextptr)?L[0][1]:L[1][1])-x,contextptr);
    } else {
        a=is_one(L[0][0])?L[0][1]:L[1][1];
        b=is_one(L[0][0])?L[1][1]:L[0][1];
        if (is_greater(b,a,contextptr))
            rest=rest*(_Heaviside(x-a,contextptr)-_Heaviside(x-b,contextptr));
        else rest=gen(0);
    }
    return lin_Heaviside(rest,x,contextptr);
}

gen cond2Heaviside(const gen &g,const identificateur &x,GIAC_CONTEXT) {
    if ((g.is_symb_of_sommet(at_and) || g.is_symb_of_sommet(at_et)) && g._SYMBptr->feuille.type==_VECT) {
        gen res(1);
        const vecteur &args=*g._SYMBptr->feuille._VECTptr;
        for (const_iterateur it=args.begin();it!=args.end();++it) {
            res=res*cond2Heaviside(*it,x,contextptr);
        }
        return res;
    }
    if (g.is_symb_of_sommet(at_ou) && g._SYMBptr->feuille.type==_VECT) {
        gen res(1);
        const vecteur &args=*g._SYMBptr->feuille._VECTptr;
        for (const_iterateur it=args.begin();it!=args.end();++it) {
            res=res*(1-cond2Heaviside(*it,x,contextptr));
        }
        return 1-res;
    }
    if (g.is_symb_of_sommet(at_inferieur_egal) || g.is_symb_of_sommet(at_inferieur_strict)) {
        if (g._SYMBptr->feuille._VECTptr->front()==x &&
                is_const_wrt_x(g._SYMBptr->feuille._VECTptr->back(),x,contextptr))
            return _Heaviside(g._SYMBptr->feuille._VECTptr->back()-x,contextptr);
        if (g._SYMBptr->feuille._VECTptr->back()==x &&
                is_const_wrt_x(g._SYMBptr->feuille._VECTptr->front(),x,contextptr))
            return _Heaviside(x-g._SYMBptr->feuille._VECTptr->front(),contextptr);
    }
    if (g.is_symb_of_sommet(at_superieur_egal) || g.is_symb_of_sommet(at_superieur_strict)) {
        if (g._SYMBptr->feuille._VECTptr->front()==x &&
                is_const_wrt_x(g._SYMBptr->feuille._VECTptr->back(),x,contextptr))
            return _Heaviside(x-g._SYMBptr->feuille._VECTptr->back(),contextptr);
        if (g._SYMBptr->feuille._VECTptr->back()==x &&
                is_const_wrt_x(g._SYMBptr->feuille._VECTptr->front(),x,contextptr))
            return _Heaviside(g._SYMBptr->feuille._VECTptr->front()-x,contextptr);
    }
    return undef;
}

gen sign2Heaviside(const gen &g,GIAC_CONTEXT) {
    if (g.type==_VECT) {
        vecteur res;
        for (const_iterateur it=g._VECTptr->begin();it!=g._VECTptr->end();++it) {
            res.push_back(sign2Heaviside(*it,contextptr));
        }
        return gen(res,g.subtype);
    }
    if (g.is_symb_of_sommet(at_sign))
        return 2*symbolic(at_Heaviside,g._SYMBptr->feuille)-1;
    if (g.type==_SYMB)
        return symbolic(g._SYMBptr->sommet,sign2Heaviside(g._SYMBptr->feuille,contextptr));
    return g;
}

gen Heaviside2sign(const gen &g,GIAC_CONTEXT) {
    if (g.type==_VECT) {
        vecteur res;
        for (const_iterateur it=g._VECTptr->begin();it!=g._VECTptr->end();++it) {
            res.push_back(Heaviside2sign(*it,contextptr));
        }
        return gen(res,g.subtype);
    }
    if (g.is_symb_of_sommet(at_Heaviside))
        return (symbolic(at_sign,g._SYMBptr->feuille)+1)/2;
    if (g.type==_SYMB)
        return symbolic(g._SYMBptr->sommet,Heaviside2sign(g._SYMBptr->feuille,contextptr));
    return g;
}

gen simplify_signs(const gen &g,const identificateur &x,GIAC_CONTEXT) {
    if (g.type==_VECT) {
        vecteur res;
        for (const_iterateur it=g._VECTptr->begin();it!=g._VECTptr->end();++it) {
            res.push_back(simplify_signs(*it,x,contextptr));
        }
        return gen(res,g.subtype);
    }
    gen a,b;
    if ((g.is_symb_of_sommet(at_Heaviside) || g.is_symb_of_sommet(at_sign)) &&
            is_linear_wrt(g._SYMBptr->feuille,x,a,b,contextptr) && !is_zero(a))
        return symbolic(g._SYMBptr->sommet,g._SYMBptr->feuille/_abs(a,contextptr));
    if (g.is_symb_of_sommet(at_Dirac) && g._SYMBptr->feuille.type!=_VECT &&
            is_linear_wrt(g._SYMBptr->feuille,x,a,b,contextptr) && !is_zero(a))
        return symbolic(at_Dirac,x+b/a)/_abs(a,contextptr);
    if (g.type==_SYMB)
        return symbolic(g._SYMBptr->sommet,simplify_signs(g._SYMBptr->feuille,x,contextptr));
    return g;
}

vecteur find_Dirac(const gen &g,const identificateur &x,gen &rest,GIAC_CONTEXT) {
    gen a,b,fac;
    rest=gen(1);
    vecteur res,factors;
    if (g.is_symb_of_sommet(at_neg)) {
        res=find_Dirac(g._SYMBptr->feuille,x,rest,contextptr);
        rest=-rest;
        return res;
    }
    if (g.is_symb_of_sommet(at_prod) && g._SYMBptr->feuille.type==_VECT)
        factors=*g._SYMBptr->feuille._VECTptr;
    else factors=vecteur(1,g);
    for (const_iterateur it=factors.begin();it!=factors.end();++it) {
        if (res.empty() && it->is_symb_of_sommet(at_Dirac)) {
            const gen &arg=(it->_SYMBptr->feuille.type==_VECT?it->_SYMBptr->feuille._VECTptr->front()
                                                             :it->_SYMBptr->feuille);
            if (is_linear_wrt(arg,x,a,b,contextptr) && !is_zero(a)) {
                res=makevecteur(-b/a,it->_SYMBptr->feuille.type==_VECT?
                                     it->_SYMBptr->feuille._VECTptr->back():gen(0));
                continue;
            }
        }
        fac=*it;
        if (fac.is_symb_of_sommet(at_abs))
            fac=fac._SYMBptr->feuille*(2*_Heaviside(fac,contextptr)-1);
        rest=rest*fac;
    }
    rest=ratnormal(rest,contextptr);
    return res;
}

/* expand f(x)*Dirac(x-a,n) to
   sum((-1)^k*binom(n,k)*(d^k f/dx^k)(a)*(d^(n-k) Dirac/dx^(n-k))(x-a),k=0..n) */
gen expand_Dirac_prod(const gen &f_orig,const identificateur &x,const gen &a,int n,GIAC_CONTEXT) {
    gen ret(0),f=f_orig;
    for (int i=0;i<=n;++i) {
        ret+=subst(f,x,a,false,contextptr)*pow(gen(-1),i)*_comb(makesequence(n,i),contextptr)*
             (i==n?_Dirac(x-a,contextptr):_Dirac(makesequence(x-a,n-i),contextptr));
        if (i<n) f=_derive(makesequence(f,x),contextptr);
    }
    return ret;
}

gen simplify_Dirac(const gen &g_orig,const identificateur &x,GIAC_CONTEXT) {
    gen g=expand(g_orig,contextptr),r,ret(0),rest(0);
    vecteur terms;
    if (g.is_symb_of_sommet(at_plus) && g._SYMBptr->feuille.type==_VECT)
        terms=*g._SYMBptr->feuille._VECTptr;
    else terms=vecteur(1,g);
    for (const_iterateur it=terms.begin();it!=terms.end();++it) {
        vecteur d=find_Dirac(*it,x,r,contextptr);
        if (d.empty()) {
            ret+=*it;
        } else {
            assert(d.size()==2 && d.back().is_integer());
            if (!is_zero(ratnormal(subst(_denom(r,contextptr),x,d.front(),false,contextptr),contextptr)))
                ret+=expand_Dirac_prod(r,x,d.front(),d.back().val,contextptr);
            else rest+=r*(is_zero(d.back())?_Dirac(x-d.front(),contextptr):
                                            _Dirac(makesequence(x-d.front(),d.back()),contextptr));
        }
    }
    if (!is_zero(rest))
        rest=simplify_Dirac(ratnormal(rest,contextptr),x,contextptr);
    return ret+rest;
}

/* return true iff g = a*(x+b)^2+c */
bool is_poly2(const gen &g,const identificateur &x,gen &a,gen &b,gen &c,GIAC_CONTEXT) {
    gen st;
    if(!ispoly(g,x,st,contextptr) || !is_one(st-1))
        return false;
    gen cf=_canonical_form(makesequence(g,x),contextptr),e,s(undef),d;
    if (cf.is_symb_of_sommet(at_plus) && cf._SYMBptr->feuille.type==_VECT &&
            cf._SYMBptr->feuille._VECTptr->size()==2) {
        e=cf._SYMBptr->feuille._VECTptr->front();
        c=cf._SYMBptr->feuille._VECTptr->back();
    } else {
        e=cf;
        c=0;
    }
    a=gen(1);
    if (e.is_symb_of_sommet(at_neg)) {
        a=gen(-1);
        e=e._SYMBptr->feuille;
    }
    if (e.is_symb_of_sommet(at_prod) && e._SYMBptr->feuille.type==_VECT) {
        const vecteur &factors=*e._SYMBptr->feuille._VECTptr;
        for (const_iterateur it=factors.begin();it!=factors.end();++it) {
            if (is_constant_wrt(*it,x,contextptr))
                a=a*(*it);
            else if (is_undef(s))
                s=*it;
            else return false;
        }
    } else s=e;
    if (s.is_symb_of_sommet(at_pow) && s._SYMBptr->feuille.type==_VECT &&
            s._SYMBptr->feuille._VECTptr->size()==2 && is_one(s._SYMBptr->feuille._VECTptr->back()-1)) {
        gen l=s._SYMBptr->feuille._VECTptr->front();
        if (!is_linear_wrt(l,x,d,b,contextptr) || !is_one(d))
            return false;
    } else return false;
    return true;
}

/* return true iff g = a*(b*x+c)^n */
bool is_monomial(const gen &g,const identificateur &x,gen &n,gen &a,gen &b,gen &c,GIAC_CONTEXT) {
    if (!ispoly(g,x,n,contextptr))
        return false;
    gen gf=factorise(makesequence(g,x),contextptr),s(undef);
    a=gen(1);
    if (gf.is_symb_of_sommet(at_prod) && gf._SYMBptr->feuille.type==_VECT) {
        const vecteur &factors=*gf._SYMBptr->feuille._VECTptr;
        for (const_iterateur it=factors.begin();it!=factors.end();++it) {
            if (is_constant_wrt(*it,x,contextptr))
                a=a*(*it);
            else if (is_undef(s))
                s=*it;
            else return false;
        }
    } else s=gf;
    return s.is_symb_of_sommet(at_pow) && s._SYMBptr->feuille.type==_VECT &&
           is_linear_wrt(s._SYMBptr->feuille._VECTptr->front(),x,b,c,contextptr);
}

/* return true iff g = a*|x+b| */
bool is_linabs(const gen &g_orig,const identificateur &x,gen &a,gen &b,GIAC_CONTEXT) {
    gen g=factorise(g_orig,contextptr),A(0),B;
    a=gen(1);
    if (g.is_symb_of_sommet(at_neg)) {
        a=gen(-1);
        g=g._SYMBptr->feuille;
    }
    vecteur factors;
    if (g.is_symb_of_sommet(at_prod) && g._SYMBptr->feuille.type==_VECT)
        factors=*g._SYMBptr->feuille._VECTptr;
    else factors=vecteur(1,g);
    for (const_iterateur it=factors.begin();it!=factors.end();++it) {
        if (is_const_wrt_x(*it,x,contextptr))
            a=a*(*it);
        else if (is_zero(A) && it->is_symb_of_sommet(at_abs) &&
                    is_linear_wrt(it->_SYMBptr->feuille,x,A,B,contextptr) && !is_zero(A)) {
            a=a*_abs(A,contextptr);
            b=B/A;
        } else return false;
    }
    return true;
}

iterateur find_lin(vecteur &lv,const gen &key,GIAC_CONTEXT) {
    iterateur it=lv.begin();
    for (;it!=lv.end();++it) {
        if (is_same(key,it->_VECTptr->front(),contextptr))
            break;
    }
    return it;
}

void extract_linpow(const gen &g,const identificateur &x,
                    const gen &e,gen &c,vecteur &lv,gen &rest,GIAC_CONTEXT) {
    gen a,b;
    if (is_const_wrt_x(g,x,contextptr))
        c=c*_pow(makesequence(g,e),contextptr);
    else if (g.is_symb_of_sommet(at_neg)) {
        c=c*_pow(makesequence(-1,e),contextptr);
        extract_linpow(g._SYMBptr->feuille,x,e,c,lv,rest,contextptr);
    } else if (g.is_symb_of_sommet(at_prod) && g._SYMBptr->feuille.type==_VECT) {
        const_iterateur it=g._SYMBptr->feuille._VECTptr->begin();
        for (;it!=g._SYMBptr->feuille._VECTptr->end();++it) {
            extract_linpow(*it,x,e,c,lv,rest,contextptr);
        }
    } else if (g.is_symb_of_sommet(at_pow)) {
        const vecteur &v=*g._SYMBptr->feuille._VECTptr;
        extract_linpow(v.front(),x,e*v.back(),c,lv,rest,contextptr);
    } else if (g.is_symb_of_sommet(at_sqrt)) {
        extract_linpow(g._SYMBptr->feuille,x,e/2,c,lv,rest,contextptr);
    } else if (g.is_symb_of_sommet(at_abs) && is_linear_wrt(g._SYMBptr->feuille,x,a,b,contextptr)) {
        c=c*_pow(makesequence(_abs(a,contextptr),e),contextptr);
        iterateur it=find_lin(lv,b/a,contextptr);
        if (it==lv.end())
            lv.push_back(makevecteur(b/a,0,e));
        else (*(it->_VECTptr))[2]+=e;
    } else if (is_linear_wrt(g,x,a,b,contextptr)) {
        c=c*_pow(makesequence(a,e),contextptr);
        iterateur it=find_lin(lv,b/a,contextptr);
        if (it==lv.end())
            lv.push_back(makevecteur(b/a,e,0));
        else (*(it->_VECTptr))[1]+=e;
    } else rest=rest*_pow(makesequence(g,e),contextptr);
}

bool is_linpow(const gen &g,const identificateur &x,gen &a,gen &b,gen &c,bool &s,GIAC_CONTEXT) {
    gen c1(1),c2(1),r1(1),r2(1),h=simplify_signs(g,x,contextptr);
    vecteur lv1,lv2;
    gen num=_numer(h,contextptr),den=_denom(h,contextptr);
    if (!is_const_wrt_x(num,x,contextptr)) num=factorise(num,contextptr);
    if (!is_const_wrt_x(den,x,contextptr)) den=factorise(den,contextptr);
    extract_linpow(num,x,1,c1,lv1,r1,contextptr);
    extract_linpow(den,x,1,c2,lv2,r2,contextptr);
    if (lv1.size()>1 || lv2.size()>1)
        return false;
    if (!lv2.empty()) {
        const vecteur &v=*lv2.front()._VECTptr;
        iterateur it=find_lin(lv1,v.front(),contextptr);
        if (it==lv1.end())
            lv1.push_back(makevecteur(v[0],-v[1],-v[2]));
        else {
            vecteur &w=*(it->_VECTptr);
            w[1]-=v[1];
            w[2]-=v[2];
        }
    }
    if (lv1.size()!=1)
        return false;
    b=ratnormal(lv1.front()._VECTptr->front(),contextptr);
    c=c1/c2;
    gen rest=r1/r2,a1=lv1.front()._VECTptr->at(1),a2=lv1.front()._VECTptr->at(2),A,B;
    if (!is_zero(im(a1,contextptr)) || !is_zero(im(a2,contextptr)) || !a1.is_integer())
        return false;
    a=a1+a2;
    if (is_strictly_greater(a,-1,contextptr)) {
        if (is_zero(_even(a1,contextptr)))
            rest=rest*_sign(x+b,contextptr);
    } else {
        if (!a2.is_integer())
            return false;
        if (is_zero(_even(a2,contextptr)))
            rest=rest*_sign(x+b,contextptr);
    }
    rest=ratnormal(rest,contextptr);
    if (is_const_wrt_x(rest,x,contextptr)) {
        c=c*rest;
        s=false;
    } else if (rest.is_symb_of_sommet(at_sign) &&
               is_linear_wrt(rest._SYMBptr->feuille,x,A,B,contextptr) && is_one(A) &&
               is_same(B/A,b,contextptr)) {
        s=true;          
    } else return false;
    return true;
}

bool is_expexp(const gen &g_orig,const identificateur &x,gen &a,gen &b,GIAC_CONTEXT) {
    gen g=expand(g_orig,contextptr);
    if (!g.is_symb_of_sommet(at_plus) || g._SYMBptr->feuille.type!=_VECT)
        return false;
    const vecteur &v=*g._SYMBptr->feuille._VECTptr;
    gen t1(0),t2(undef),a1,b1,a2,b2;
    for (const_iterateur it=v.begin();it!=v.end();++it) {
        if (is_undef(t2) && it->is_symb_of_sommet(at_neg) &&
                 it->_SYMBptr->feuille.is_symb_of_sommet(at_exp) &&
                 is_linear_wrt(it->_SYMBptr->feuille._SYMBptr->feuille,x,a2,b2,contextptr))
            t2=*it;
        else t1+=*it;
    }
    if (!is_linear_wrt(t1,x,a1,b1,contextptr) || !is_zero(b1) || !is_same(a1,a2,contextptr))
        return false;
    a=a1;
    b=b2;
    return true;
}

int is_expsum(const gen &g_orig,const identificateur &x,gen &a,gen &b,gen &c,bool &sq,GIAC_CONTEXT) {
    gen g=factorise(_lin(expand(g_orig,contextptr),contextptr),contextptr);
    c=gen(1);
    if (g.is_symb_of_sommet(at_neg)) {
        c=gen(-1);
        g=g._SYMBptr->feuille;
    }
    vecteur factors;
    vecteur A(2),B(2),S(3);
    gen rpart,ipart,p;
    int i=0;
    sq=false;
    if (g.is_symb_of_sommet(at_prod))
        factors=*g._SYMBptr->feuille._VECTptr;
    else factors=vecteur(1,g);
    for (const_iterateur it=factors.begin();it!=factors.end();++it) {
        if (is_const_wrt_x(*it,x,contextptr))
            c=c*(*it);
        else if (it->is_symb_of_sommet(at_plus) && it->_SYMBptr->feuille.type==_VECT &&
                 it->_SYMBptr->feuille._VECTptr->size()>=2) {
            rpart=re(*it,contextptr);
            ipart=im(*it,contextptr);
            if (is_zero(rpart)) {
                p=ipart;
                c=c*cst_i;
            } else if (is_zero(ipart))
                p=rpart;
            else return 0;
            if (!p.is_symb_of_sommet(at_plus) || p._SYMBptr->feuille.type!=_VECT ||
                    p._SYMBptr->feuille._VECTptr->size()<2)
                return 0;
            const vecteur &terms=*(p._SYMBptr->feuille._VECTptr);
            for (const_iterateur jt=terms.begin();jt!=terms.end();++jt) {
                if (i<2 && jt->is_symb_of_sommet(at_exp) &&
                        is_linear_wrt(jt->_SYMBptr->feuille,x,A[i],B[i],contextptr)) {
                    S[i]=gen(1);
                    ++i;
                } else if (i<2 && jt->is_symb_of_sommet(at_neg) &&
                        jt->_SYMBptr->feuille.is_symb_of_sommet(at_exp) &&
                        is_linear_wrt(jt->_SYMBptr->feuille._SYMBptr->feuille,x,A[i],B[i],contextptr)) {
                    S[i]=gen(-1);
                    ++i;
                } else if (!sq && is_one(_abs(S[2]=*jt/2,contextptr)) && terms.size()==3)
                    sq=true;
                else return 0;
            }
        } else return 0;
    }
    if (i<2 || !is_zero(ratnormal(A[0]+A[1],contextptr)) || !is_zero(ratnormal(B[0]+B[1],contextptr)))
        return 0;
    if (sq) {
        if (!is_one(S[0]) || !is_one(S[1]))
            return false;
        a=A[0]/2;
        b=B[0]/2;
        return 1+(1+S[2]).val/2;
    }
    c=c*S[0];
    a=A[0];
    b=B[0];
    return 1+int(is_one(S[0]*S[1]));
}

bool is_integral(const gen &g,identificateur &x,gen &f,gen &a,gen &b,GIAC_CONTEXT) {
    if ((!g.is_symb_of_sommet(at_Int) && !g.is_symb_of_sommet(at_int) &&
            !g.is_symb_of_sommet(at_integrate)) || g._SYMBptr->feuille.type!=_VECT)
        return false;
    const vecteur &args=*g._SYMBptr->feuille._VECTptr;
    if (args.size()!=2 && args.size()!=4)
        return false;
    f=args.front();
    if (args.size()==2) {
        if (!args.back().is_symb_of_sommet(at_equal) ||
                args.back()._SYMBptr->feuille._VECTptr->front().type!=_IDNT ||
                !args.back()._SYMBptr->feuille._VECTptr->back().is_symb_of_sommet(at_interval))
            return false;
        x=*args.back()._SYMBptr->feuille._VECTptr->front()._IDNTptr;
        a=args.back()._SYMBptr->feuille._VECTptr->back()._SYMBptr->feuille._VECTptr->front();
        b=args.back()._SYMBptr->feuille._VECTptr->back()._SYMBptr->feuille._VECTptr->back();
        return true;
    }
    if (args[1].type!=_IDNT)
        return false;
    x=*args[1]._IDNTptr;
    a=args[2];
    b=args[3];
    return true;
}

bool is_convolution(const gen &e,const identificateur &x,gen &f,gen &g,GIAC_CONTEXT) {
    gen a,b,h;
    identificateur t;
    if (!is_integral(e,t,h,a,b,contextptr) || !a.is_symb_of_sommet(at_neg) ||
            !is_inf(a._SYMBptr->feuille) || !b.is_symb_of_sommet(at_plus) ||
            !is_inf(b._SYMBptr->feuille))
        return false;
    vecteur factors;
    if (h.is_symb_of_sommet(at_prod) && h._SYMBptr->feuille.type==_VECT)
        factors=*h._SYMBptr->feuille._VECTptr;
    else factors=vecteur(1,h);
    f=g=gen(1);
    for (const_iterateur it=factors.begin();it!=factors.end();++it) {
        if (is_const_wrt_x(*it,x,contextptr))
            f=f*(*it);
        else g=g*(*it);
    }
    g=recursive_normal(subst(g,x,x+t,false,contextptr),contextptr);
    return is_const_wrt_x(g,t,contextptr);
}

int fourier_func_type(const gen &g,const identificateur &x,vecteur &params,GIAC_CONTEXT) {
    params.clear();
    if (is_const_wrt_x(g,x,contextptr)) {
        params.push_back(g);
        return _FOURIER_FUNCTYPE_ONE;
    }
    gen a,b,c,d,n;
    vecteur tbl,vars,deg;
    identificateur fcn;
    bool sgn,sq;
    int dg,hyp;
    if (is_rational_wrt(g,x)) {
        gen num=_numer(g,contextptr),den=_denom(g,contextptr);
        assert(is_constant_wrt(num,x,contextptr));
        if (is_linear_wrt(den,x,a,b,contextptr)) {
            params=makevecteur(num/a,b/a,1);
        } else if (is_monomial(den,x,n,a,b,c,contextptr)) {
            assert(!is_zero(a) && !is_zero(b));
            params=makevecteur(num/(a*_pow(makesequence(b,n),contextptr)),c/b,n);
        } else return _FOURIER_FUNCTYPE_UNKNOWN;
        return _FOURIER_FUNCTYPE_INV_MONOM;
    }
    if (g.is_symb_of_sommet(at_sign) && is_linear_wrt(g._SYMBptr->feuille,x,a,b,contextptr) &&
            is_zero(im(a,contextptr)) && is_zero(im(b,contextptr))) {
        params=makevecteur(a,b);
        return _FOURIER_FUNCTYPE_SGN;
    }
    if (g.is_symb_of_sommet(at_Dirac) && ((g._SYMBptr->feuille.type==_VECT &&
            is_linear_wrt(g._SYMBptr->feuille._VECTptr->front(),x,a,b,contextptr)) ||
            is_linear_wrt(g._SYMBptr->feuille,x,a,b,contextptr)) &&
            is_zero(im(a,contextptr)) && is_zero(im(b,contextptr))) {
        params=makevecteur(a,b/a,
            g._SYMBptr->feuille.type==_VECT?g._SYMBptr->feuille._VECTptr->back():gen(0));
        return _FOURIER_FUNCTYPE_DIRAC;
    }
    if (g.is_symb_of_sommet(at_BesselJ) && g._SYMBptr->feuille.type==_VECT &&
            g._SYMBptr->feuille._VECTptr->size()==2 &&
            (c=g._SYMBptr->feuille._VECTptr->front()).is_integer() && is_positive(c,contextptr) &&
            is_linear_wrt(g._SYMBptr->feuille._VECTptr->back(),x,a,b,contextptr) &&
            is_zero(im(a,contextptr)) && is_zero(im(b,contextptr))) {
        params=makevecteur(a,b,c);
        return _FOURIER_FUNCTYPE_BESSELJ;
    }
    if (g.is_symb_of_sommet(at_ln) && g._SYMBptr->feuille.is_symb_of_sommet(at_abs) &&
            is_linear_wrt(g._SYMBptr->feuille._SYMBptr->feuille,x,a,b,contextptr) &&
            is_zero(im(a,contextptr)) && is_zero(im(b,contextptr))) {
        params.push_back(b/a);
        return _FOURIER_FUNCTYPE_LOGABSX;
    }
    if (g.is_symb_of_sommet(at_inv) && g._SYMBptr->feuille.is_symb_of_sommet(at_abs) &&
            is_linear_wrt(g._SYMBptr->feuille._SYMBptr->feuille,x,a,b,contextptr) &&
            is_zero(im(a,contextptr)) && is_zero(im(b,contextptr))) {
        params=makevecteur(symbolic(at_inv,_abs(a,contextptr)),b/a);
        return _FOURIER_FUNCTYPE_INVABSX;
    }
    if (is_linpow(g,x,a,b,c,sgn,contextptr) && is_zero(im(b,contextptr))) {
        if (!sgn && is_greater(-1,a,contextptr)) {
            params=makevecteur(c,b,-a);
            return _FOURIER_FUNCTYPE_INV_MONOM;
        }
        params=makevecteur(a,b,c,sgn?gen(1):gen(0));
        return _FOURIER_FUNCTYPE_ABSX_ALPHA;
    }
    if (g.is_symb_of_sommet(at_exp) &&
            is_poly2(g._SYMBptr->feuille,x,a,b,c,contextptr) &&
            is_zero(im(b,contextptr)) && is_positive(re(-a,contextptr),contextptr)) {
        params=makevecteur(exp(c,contextptr),b,-a);
        return _FOURIER_FUNCTYPE_GAUSSIAN;
    }
    if (g.is_symb_of_sommet(at_Gamma) && is_one(re(g._SYMBptr->feuille,contextptr)) &&
            is_linear_wrt(im(g._SYMBptr->feuille,contextptr),x,a,b,contextptr)) {
        params=makevecteur(a,b/a);
        return _FOURIER_FUNCTYPE_GAMMA;
    }
    if (g.is_symb_of_sommet(at_prod) && g._SYMBptr->feuille.type==_VECT &&
            g._SYMBptr->feuille._VECTptr->size()==2 &&
            ((g._SYMBptr->feuille._VECTptr->front().is_symb_of_sommet(at_exp) &&
              is_linear_wrt(g._SYMBptr->feuille._VECTptr->front()._SYMBptr->feuille,x,a,d,contextptr) &&
              g._SYMBptr->feuille._VECTptr->back().is_symb_of_sommet(at_Heaviside) &&
              is_linear_wrt(g._SYMBptr->feuille._VECTptr->back()._SYMBptr->feuille,x,b,c,contextptr)) ||
             (g._SYMBptr->feuille._VECTptr->back().is_symb_of_sommet(at_exp) &&
              is_linear_wrt(g._SYMBptr->feuille._VECTptr->back()._SYMBptr->feuille,x,a,d,contextptr) &&
              g._SYMBptr->feuille._VECTptr->front().is_symb_of_sommet(at_Heaviside) &&
              is_linear_wrt(g._SYMBptr->feuille._VECTptr->front()._SYMBptr->feuille,x,b,c,contextptr)))
              && is_zero(im(b,contextptr)) && is_zero(im(c,contextptr)) && !is_zero(b) && is_zero(d) &&
              is_strictly_positive(-a/b,contextptr)) {
        params=makevecteur(a,b,c);
        return _FOURIER_FUNCTYPE_EXP_HEAVISIDE;
    }
    if (g.is_symb_of_sommet(at_Heaviside) && is_linear_wrt(g._SYMBptr->feuille,x,a,b,contextptr) &&
            is_zero(im(a,contextptr)) && is_zero(im(b,contextptr))) {
        params=makevecteur(a,b/a);
        return _FOURIER_FUNCTYPE_HEAVISIDE;
    }
    if (g.is_symb_of_sommet(at_exp) && is_linabs(g._SYMBptr->feuille,x,a,b,contextptr) &&
            is_strictly_positive(-a,contextptr)) {
        params=makevecteur(-a,b);
        return _FOURIER_FUNCTYPE_EXPABSX;
    }
    if (g.is_symb_of_sommet(at_exp) && is_expexp(g._SYMBptr->feuille,x,a,b,contextptr) &&
            is_zero(im(a,contextptr)) && is_zero(im(b,contextptr))) {
        params=makevecteur(a,b);
        return _FOURIER_FUNCTYPE_EXPEXP;
    }
    if (g.is_symb_of_sommet(at_Airy_Ai) && is_linear_wrt(g._SYMBptr->feuille,x,a,b,contextptr) &&
            is_zero(im(a,contextptr)) && is_zero(im(b,contextptr))) {
        params=makevecteur(a,b/a);
        return _FOURIER_FUNCTYPE_AIRY_AI;
    }
    if (g.is_symb_of_sommet(at_inv) && (hyp=is_expsum(g._SYMBptr->feuille,x,a,b,c,sq,contextptr))!=0) {
        params=makevecteur(c*gen(sq?4:2),a,b/a);
        if (hyp==1 && !sq) return _FOURIER_FUNCTYPE_COSECH;
        if (hyp==2) return sq?_FOURIER_FUNCTYPE_SECH_2:_FOURIER_FUNCTYPE_SECH;
        return _FOURIER_FUNCTYPE_UNKNOWN;
    }
    if (is_convolution(g,x,a,b,contextptr)) {
        params=makevecteur(a,b);
        return _FOURIER_FUNCTYPE_CONVOLUTION;
    }
    if ((g.is_symb_of_sommet(at_piecewise) || g.is_symb_of_sommet(at_when)) &&
            g._SYMBptr->feuille.type==_VECT) {
        const vecteur &args=*g._SYMBptr->feuille._VECTptr;
        bool cnd=true;
        for (const_iterateur it=args.begin();it!=args.end();++it) {
            if (cnd) {
                if (it+1==args.end()) {
                    params.push_back(symbolic(at_plus,unsigned_inf));
                    params.push_back(*it);
                } else if ((it->is_symb_of_sommet(at_inferieur_egal) ||
                            it->is_symb_of_sommet(at_inferieur_strict)) &&
                           it->_SYMBptr->feuille.type==_VECT &&
                           it->_SYMBptr->feuille._VECTptr->front()==x &&
                           is_const_wrt_x(it->_SYMBptr->feuille._VECTptr->back(),x,contextptr))
                    params.push_back(it->_SYMBptr->feuille._VECTptr->back());
                else if ((it->is_symb_of_sommet(at_superieur_egal) ||
                          it->is_symb_of_sommet(at_superieur_strict)) &&
                         it->_SYMBptr->feuille.type==_VECT &&
                         it->_SYMBptr->feuille._VECTptr->back()==x &&
                         is_const_wrt_x(it->_SYMBptr->feuille._VECTptr->front(),x,contextptr))
                    params.push_back(it->_SYMBptr->feuille._VECTptr->front());
                else return _FOURIER_FUNCTYPE_UNKNOWN;
            } else params.push_back(*it);
            cnd=!cnd;
        }
        assert(params.size()%2==0);
        int i,n=params.size()/2;
        for (i=0;i<n && (i==0 || is_greater(params[2*i],params[2*(i-1)],contextptr));++i);
        return (n>1 && i==n)?_FOURIER_FUNCTYPE_PIECEWISE:_FOURIER_FUNCTYPE_UNKNOWN;
    }
    if (is_diff(g,x,a,dg)) {
        params=makevecteur(a,dg);
        return _FOURIER_FUNCTYPE_DIFF;
    }
    if (is_partialdiff(g,fcn,vars,deg,contextptr) &&
            !(tbl=fourier_table_find(fcn,vars.size())).empty() &&
            is_linear_wrt(vars[tbl[2].val],x,a,b,contextptr)){
        params=mergevecteur(mergevecteur(tbl,makevecteur(a,b)),mergevecteur(vars,deg));
        return _FOURIER_FUNCTYPE_PARTIAL_DIFF;
    }
    if (is_func(g,fcn,vars) && !(tbl=fourier_table_find(fcn,vars.size())).empty() &&
            is_linear_wrt(vars[tbl[2].val],x,a,b,contextptr)) {        
        params=mergevecteur(tbl,mergevecteur(makevecteur(a,b),vars));
        return _FOURIER_FUNCTYPE_FUNC;
    }
    // some functions with denominator equal to ax+b
    gen gnorm=ratnormal(g,contextptr);
    if (is_linear_wrt(_denom(gnorm,contextptr),x,c,d,contextptr) && !is_zero(c)) {
        gen h=_numer(gnorm,contextptr);
        if (h.is_symb_of_sommet(at_atan) && is_linear_wrt(h._SYMBptr->feuille,x,a,b,contextptr) &&
                !is_zero(a) && is_same(b*c,a*d,contextptr)) {
            params=makevecteur(a,c,b/a);
            return _FOURIER_FUNCTYPE_ATAN_OVERX;
        }
        if (h.is_symb_of_sommet(at_exp) && is_linabs(h._SYMBptr->feuille,x,a,b,contextptr) &&
                is_same(b*c,d,contextptr) && is_strictly_positive(-a,contextptr)) {
            params=makevecteur(-a,b,c);
            return _FOURIER_FUNCTYPE_EXPABSX_OVERX;
        }
    }
    return _FOURIER_FUNCTYPE_UNKNOWN;
}

gen derive_FT(const gen &g,const identificateur &x,int n,GIAC_CONTEXT) {
    gen d=g;
    for (int i=0;i<n;++i) {
        d=_derive(makesequence(simplify_signs(sign2Heaviside(d,contextptr),x,contextptr),x),contextptr);
        //d=simplify_Dirac(d,x,contextptr);
    }
    return d;
}

gen fourier(const gen &f_orig,const identificateur &x,const identificateur &s,
            bool neg,bool try_diff,GIAC_CONTEXT) {
    gen fsimp;
    bool do_simp=!has_integral(f_orig) && !has_partial_diff(f_orig);
    if (do_simp) {
        fsimp=lin_Heaviside(sign2Heaviside(simplify_signs(f_orig,x,contextptr),contextptr),x,contextptr);
    } else fsimp=f_orig;
    vecteur terms=fourier_terms(fsimp/(neg?2*cst_pi:gen(1)),x,do_simp,contextptr),p;
    gen ret(0),intgr(0),t,a,b,c,n,minf=symbolic(at_neg,unsigned_inf),pinf=symbolic(at_plus,unsigned_inf);
    gen rest(0);
    vecteur vars,degr;
    int ind,sz;
    for (int ti=0;ti<int(terms.size());++ti) {
        const gen &term=terms[ti];
        assert(term.type==_VECT);
        const vecteur &v=*term._VECTptr;
        gen cnst=v[0],sh=v[1],d=v[2],f=v[3];
        assert(d.is_integer() && is_positive(d,contextptr));
        switch (fourier_func_type(f,x,p,contextptr)) {
        case _FOURIER_FUNCTYPE_PIECEWISE:
            sz=p.size()/2;
            a=p[0];
            b=_Heaviside(a-x,contextptr)*p[1]+_Heaviside(x-p[2*sz-4],contextptr)*p[2*sz-1];
            t=fourier(b,x,s,false,true,contextptr);
            for (ind=1;ind<sz-1;++ind) {
                b=p[2*ind];
                t+=_integrate(makesequence(exp(-cst_i*x*s,contextptr)*p[2*ind+1],x,a,b),contextptr);
                a=b;
            }
            break;
        case _FOURIER_FUNCTYPE_FUNC:
            a=p[3];
            b=p[4];
            c=p[0];
            vars=vecteur(p.begin()+5,p.end());
            vars[p[2].val]=(is_one(c)?s:-s)/a;
            if (vars.size()==1) vars=makevecteur(p[1],vars.front());
            else vars=makevecteur(p[1],gen(vars,_SEQ__VECT));
            t=c*symbolic(at_of,vars)*exp(cst_i*s*b/a,contextptr)/_abs(a,contextptr);
            break;
        case _FOURIER_FUNCTYPE_DIFF:
            t=_pow(makesequence(cst_i*s,p[1]),contextptr)*fourier(p[0],x,s,false,false,contextptr);
            break;
        case _FOURIER_FUNCTYPE_PARTIAL_DIFF:
            a=p[3];
            b=p[4];
            c=p[0];
            vars=vecteur(p.begin()+5,p.end()-(p.size()-5)/2);
            vars[p[2].val]=(is_one(c)?s:-s)/a;
            vars=makevecteur(p[1],gen(vars,_SEQ__VECT));
            degr=vecteur(p.begin()+5+(p.size()-5)/2,p.end());
            t=c*symbolic(at_of,vars)*exp(cst_i*s*b/a,contextptr)/_abs(a,contextptr);
            t=t*_pow(makesequence(cst_i*s/a,degr[p[2].val]),contextptr);
            for (int i=0;i<int(degr.size());++i) {
                if (i==p[2].val) continue;
                t=_derive(makesequence(t,vars[i],degr[i]),contextptr);
            }
            break;
        case _FOURIER_FUNCTYPE_ONE:
            t=2*p[0]*cst_pi*_Dirac(s,contextptr);
            break;
        case _FOURIER_FUNCTYPE_DIRAC:
            t=_pow(makesequence(cst_i*s/p[0],p[2]),contextptr)*exp(cst_i*p[1]*s,contextptr)/
              _abs(p[0],contextptr);
            break;
        case _FOURIER_FUNCTYPE_HEAVISIDE:
            t=exp(cst_i*s*p[1],contextptr)*
              (_sign(p[0],contextptr)/(cst_i*s)+cst_pi*_Dirac(s,contextptr));
            break;
        case _FOURIER_FUNCTYPE_EXP_HEAVISIDE:
            t=exp(p[2]*(cst_i*s-p[0])/p[1],contextptr)*_sign(p[1],contextptr)/(cst_i*s-p[0]);
            break;
        case _FOURIER_FUNCTYPE_BESSELJ:
            t=2*_pow(makesequence(-cst_i,p[2]),contextptr)*exp(cst_i*p[1]*s/p[0],contextptr)*
              _tchebyshev1(makesequence(p[2],s/p[0]),contextptr)*_rect(s/(2*p[0]),contextptr)/
              (_abs(p[0],contextptr)*sqrt(1-pow(s/p[0],2),contextptr));
            break;
        case _FOURIER_FUNCTYPE_LOGABSX:
            t=-exp(cst_i*p[0]*s,contextptr)*cst_pi*
              (_inv(_abs(s,contextptr),contextptr)+2*cst_euler_gamma*_Dirac(s,contextptr));
            break;
        case _FOURIER_FUNCTYPE_INVABSX:
            t=-2*p[0]*exp(cst_i*p[1]*s,contextptr)*(ln(_abs(s,contextptr),contextptr)+cst_euler_gamma);
            break;
        case _FOURIER_FUNCTYPE_GAUSSIAN:
            if (is_strictly_positive(re(p[2],contextptr),contextptr))
                t=p[0]*exp(cst_i*p[1]*s,contextptr)*
                  sqrt(cst_pi/p[2],contextptr)*exp(-pow(s,2)/(4*p[2]),contextptr);
            else {
                b=im(-p[2],contextptr);
                a=_sign(b,contextptr);
                b=_abs(b,contextptr);
                assert(!is_zero(b));
                t=p[0]*sqrt(cst_pi/b,contextptr)*exp(-a*cst_i*(pow(s,2)/(4*b)-cst_pi/4),contextptr);
            }
            break;
        case _FOURIER_FUNCTYPE_GAMMA:
            t=2*cst_pi*exp(cst_i*p[1]*s-s/p[0]-exp(-s/p[0],contextptr),contextptr)/
              _abs(p[0],contextptr);
            break;
        case _FOURIER_FUNCTYPE_EXPABSX:
            t=2*p[0]*exp(cst_i*p[1]*s,contextptr)/(pow(p[0],2)+pow(s,2));
            break;
        case _FOURIER_FUNCTYPE_EXPEXP:
            t=exp(cst_i*p[1]*s/p[0]-p[1],contextptr)*
              _Gamma(cst_i*s/p[0]+1,contextptr)/_abs(p[0],contextptr);
            break;
        case _FOURIER_FUNCTYPE_AIRY_AI:
            t=exp(cst_i*(p[1]*s+pow(s/p[0],3)/3),contextptr)/_abs(p[0],contextptr);
            break;
        case _FOURIER_FUNCTYPE_ATAN_OVERX:
            t=cst_pi*_sign(p[0],contextptr)*exp(cst_i*p[2]*s,contextptr)*
              _upper_incomplete_gamma(makesequence(0,_abs(s/p[0],contextptr)),contextptr)/p[1];
            break;
        case _FOURIER_FUNCTYPE_EXPABSX_OVERX:
            t=-2*cst_i*atan(s/p[0],contextptr)*exp(cst_i*p[1]*s,contextptr)/p[2];
            break;
        case _FOURIER_FUNCTYPE_COSECH:
            t=-cst_i*cst_pi*exp(cst_i*s*p[2],contextptr)*tanh(cst_pi*s/(2*p[1]),contextptr)/
              (_abs(p[1],contextptr)*p[0]);
            break;
        case _FOURIER_FUNCTYPE_SECH:
            t=cst_pi*exp(cst_i*p[2]*s,contextptr)/
              (p[0]*cosh(cst_pi*s/(2*p[1]),contextptr)*_abs(p[1],contextptr));
            break;
        case _FOURIER_FUNCTYPE_SECH_2:
            t=s*cst_pi*exp(cst_i*p[2]*s,contextptr)/
              (p[0]*p[1]*sinh(cst_pi*s/(2*p[1]),contextptr)*_abs(p[1],contextptr));
            break;
        case _FOURIER_FUNCTYPE_CONVOLUTION:
            t=fourier(p[0],x,s,false,true,contextptr)*fourier(p[1],x,s,false,true,contextptr);
            break;
        case _FOURIER_FUNCTYPE_INV_MONOM:
        case _FOURIER_FUNCTYPE_ABSX_ALPHA:
        default:
            rest+=cnst*exp(cst_i*sh*x,contextptr)*pow(x,d.val)*f;
            continue;
        }
        t=cnst*subst(_pow(makesequence(cst_i,d),contextptr)*derive_FT(t,s,d.val,contextptr),
                     s,s-sh,false,contextptr);
        if (neg) t=subst(t,s,-s,false,contextptr);
        ret+=_exp2pow(_lin(recursive_normal(t,contextptr),contextptr),contextptr);
    }
    if (!is_zero(rest)) {
        terms=fourier_terms(Heaviside2sign(rest,contextptr),x,do_simp,contextptr);
        for (const_iterateur it=terms.begin();it!=terms.end();++it) {
            assert(it->type==_VECT);
            const vecteur &v=*(it->_VECTptr);
            gen cnst=v[0],sh=v[1],d=v[2],f=v[3];
            assert(d.is_integer() && is_positive(d,contextptr));
            switch (fourier_func_type(f,x,p,contextptr)) {
            case _FOURIER_FUNCTYPE_SGN:
                t=-2*cst_i*exp(cst_i*p[1]*s/p[0],contextptr)*_sign(p[0],contextptr)/s;
                break;
            case _FOURIER_FUNCTYPE_INV_MONOM:
                n=p[2]-1;
                if (is_zero(im(p[1],contextptr))) {
                    t=_pow(makesequence(-cst_i*s,n),contextptr)*
                      exp(cst_i*p[1]*s,contextptr)*_sign(s,contextptr);
                } else {
                    gen sgn=_sign(im(p[1],contextptr),contextptr);
                    t=2*sgn*_pow(makesequence(-cst_i*s,n),contextptr)*
                      exp(cst_i*p[1]*s,contextptr)*_Heaviside(sgn*s,contextptr);
                }
                t=-t*p[0]*cst_pi*cst_i/_factorial(n,contextptr);
                break;
            case _FOURIER_FUNCTYPE_ABSX_ALPHA:
                t=p[2]*exp(cst_i*p[1]*s,contextptr);
                if (is_greater(-1,p[0],contextptr)) {
                    assert(p[0].is_integer() && is_one(p[3]));
                    n=-p[0]-1;
                    t=-2*t*_pow(makesequence(-cst_i*s,n),contextptr)*
                      (ln(_abs(s,contextptr),contextptr)+cst_euler_gamma)/_factorial(n,contextptr);
                } else {
                    t=-2*t*_Gamma(p[0]+1,contextptr)/
                      _pow(makesequence(_abs(s,contextptr),p[0]+1),contextptr);
                    if (is_one(p[3]))
                        t=t*cst_i*_sign(s,contextptr)*cos(cst_pi*p[0]/2,contextptr);
                    else t=t*sin(cst_pi*p[0]/2,contextptr);
                }
                break;
            default:
                if (!try_diff)
                    return undef;
                f=sign2Heaviside(f,contextptr);
                if (try_diff) {
                    // check whether we can transform the derivative
                    gen df=derive_FT(f,x,1,contextptr);
                    gen F=fourier(df,x,s,false,false,contextptr);
                    if (!is_undef(F) && !is_inf(F)) {
                        t=-cst_i*F/s;
                        break;
                    }
                }
                *logptr(contextptr) << "Unable to determine Fourier transform of "
                                    << f << ", resorting to integration\n";
                intgr+=cnst*eval(_Int(makesequence(exp(-cst_i*x*((neg?-s:s)-sh),contextptr)*
                                        pow(x,d.val)*f,x,minf,pinf),contextptr),0,contextptr);
                continue;
            }
            t=cnst*subst(_pow(makesequence(cst_i,d),contextptr)*derive_FT(t,s,d.val,contextptr),
                         s,s-sh,false,contextptr);
            if (neg) t=subst(t,s,-s,false,contextptr);
            ret+=_exp2pow(_lin(recursive_normal(t,contextptr),contextptr),contextptr);
        }
    }
    if (is_zero(recursive_normal(im(ret,contextptr),contextptr)))
        ret=re(ret,contextptr);
    ret=simplify_signs(ret,s,contextptr);
    ret=simplify_Dirac(ret,s,contextptr);
    ret=Heaviside2abs(ret,s,contextptr);
    gen rcn=recursive_normal(ret,contextptr);
    if (is_rational_wrt(rcn,s))
        ret=simplify(ret,contextptr);
    else if (is_simpler(rcn,ret))
        ret=rcn;
    if (!is_const_wrt_x(ret,s,contextptr)) {
        gen rcn1=ratnormal(sign2Heaviside(ret,contextptr),contextptr);
        gen rcn2=ratnormal(Heaviside2sign(ret,contextptr),contextptr);
        ret=is_simpler(rcn1,rcn2)?rcn1:rcn2;
        rcn=ratnormal(_exp2trig(ret,contextptr),contextptr);
        if (is_simpler(rcn,ret)) ret=rcn;
        rcn=factorise(ret,contextptr);
        if (is_simpler(rcn,ret)) ret=rcn;
    }
    return ret+intgr;
}

gen _fourier(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    gen f_orig;
    identificateur var("x"),tvar(" fourier_s");
    bool has_tvar=false;
    if (g.type==_VECT) {
        if (g.subtype!=_SEQ__VECT)
            return gentypeerr(contextptr);
        const vecteur &args=*g._VECTptr;
        if (args.size()>3 || args.empty())
            return gensizeerr(contextptr);
        f_orig=args.front();
        if (args.size()>=2) {
            if (args[1].type!=_IDNT)
                return gentypeerr(contextptr);
            var=*args[1]._IDNTptr;
        }
        if (args.size()==3) {
            if (args[2].type!=_IDNT)
                return gentypeerr(contextptr);
            if ((tvar=*args[2]._IDNTptr)==var)
                return gensizeerr(contextptr);
            has_tvar=true;
        }
    } else f_orig=g;
    gen ret=fourier(f_orig,var,tvar,false,true,contextptr);
    if (!has_tvar)
        ret=subst(ret,tvar,var,false,contextptr);
    return ret;
}
static const char _fourier_s []="fourier";
static define_unary_function_eval (__fourier,&_fourier,_fourier_s);
define_unary_function_ptr5(at_fourier,alias_at_fourier,&__fourier,0,true)

gen _ifourier(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    gen f_orig;
    identificateur var("x"),tvar(" fourier_s");
    bool has_tvar=false;
    if (g.type==_VECT) {
        if (g.subtype!=_SEQ__VECT)
            return gentypeerr(contextptr);
        const vecteur &args=*g._VECTptr;
        if (args.size()>3 || args.empty())
            return gensizeerr(contextptr);
        f_orig=args.front();
        if (args.size()>=2) {
            if (args[1].type!=_IDNT)
                return gentypeerr(contextptr);
            var=*args[1]._IDNTptr;
        }
        if (args.size()==3) {
            if (args[2].type!=_IDNT)
                return gentypeerr(contextptr);
            if ((tvar=*args[2]._IDNTptr)==var)
                return gensizeerr(contextptr);
            has_tvar=true;
        }
    } else f_orig=g;
    gen ret=fourier(f_orig,var,tvar,true,true,contextptr);
    if (!has_tvar)
        ret=subst(ret,tvar,var,false,contextptr);
    return ret;
}
static const char _ifourier_s []="ifourier";
static define_unary_function_eval (__ifourier,&_ifourier,_ifourier_s);
define_unary_function_ptr5(at_ifourier,alias_at_ifourier,&__ifourier,0,true)

vecteur apply_window_function(const gen &expr,const identificateur &k,const vecteur &data,int start,int len,GIAC_CONTEXT) {
    vecteur output(len);
    for (int j=0;j<len;++j) {
        output[j]=_evalf(subst(expr,k,gen((double)j),false,contextptr),contextptr)*data[start+j];
    }
    return output;
}

bool nivelate(vecteur &data,int k,const gen &b,const gen &val,const unary_function_ptr *comp,GIAC_CONTEXT) {
    gen r;
    if (has_i(data[k]) && !is_zero((r=_abs(data[k],contextptr)))) {
        if (_eval(symbolic(comp,makevecteur(r,b)),contextptr).val!=0) {
            data[k]=val*data[k]/r;
            return true;
        }
    } else if (_eval(symbolic(comp,makevecteur(data[k],b)),contextptr).val!=0) {
        data[k]=val;
        return true;
    }
    return false;
}

gen _threshold(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT || g.subtype!=_SEQ__VECT)
        return gentypeerr(contextptr);
    vecteur &args=*g._VECTptr;
    if (int(args.size())<2)
        return gensizeerr(contextptr);
    if (args.front().type!=_VECT)
        return gentypeerr(contextptr);
    vecteur &data=*args.front()._VECTptr;
    gen &bnd=args.at(1);
    int n=data.size();
    vecteur output=data;
    if (bnd.type==_VECT) {
        if (int(bnd._VECTptr->size())!=2)
            return gensizeerr(contextptr);
        gen lb=bnd._VECTptr->front(),ub=bnd._VECTptr->back(),lval,uval;
        if (lb.is_symb_of_sommet(at_equal)) {
            lval=_rhs(lb,contextptr);
            lb=_lhs(lb,contextptr);
        } else lval=lb;
        if (ub.is_symb_of_sommet(at_equal)) {
            uval=_rhs(ub,contextptr);
            ub=_lhs(ub,contextptr);
        } else uval=ub;
        for (int k=0;k<n;++k) {
            if (!nivelate(output,k,lb,lval,at_inferieur_strict,contextptr))
                nivelate(output,k,ub,uval,at_superieur_strict,contextptr);
        }
    } else {
        gen val;
        if (bnd.is_symb_of_sommet(at_equal)) {
            val=_rhs(bnd,contextptr);
            bnd=_lhs(bnd,contextptr);
        } else val=bnd;
        if (!is_real(bnd,contextptr))
            return gentypeerr(contextptr);
        gen comp=at_inferieur_strict,isabs;
        bool absolute=false;
        for (const_iterateur it=args.begin()+2;it!=args.end();++it) {
            if (*it==at_superieur_strict || *it==at_superieur_egal || *it==at_inferieur_egal)
                comp=*it;
            isabs=gen(1);
            if (*it==at_abs || (it->is_symb_of_sommet(at_equal) &&
                                it->_SYMBptr->feuille._VECTptr->front()==at_abs &&
                                (isabs=it->_SYMBptr->feuille._VECTptr->back()).type==_INT_ &&
                                isabs.subtype==_INT_BOOLEAN)) {
                if (has_i(data) || !is_strictly_positive(bnd,contextptr))
                    return gentypeerr(contextptr);
                absolute=(bool)isabs.val;
            }
        }
        for (int k=0;k<n;++k) {
            if (absolute) {
                if (_eval(symbolic(comp._FUNCptr,makevecteur(_abs(data[k],contextptr),bnd)),contextptr).val!=0)
                    output[k]=is_positive(data[k],contextptr)?val:-val;
            } else nivelate(output,k,bnd,val,comp._FUNCptr,contextptr);
        }
    }
    return _eval(output,contextptr);
}
static const char _threshold_s []="threshold";
static define_unary_function_eval (__threshold,&_threshold,_threshold_s);
define_unary_function_ptr5(at_threshold,alias_at_threshold,&__threshold,0,true)

bool parse_window_parameters(const gen &g,vecteur &data,int &start,int &len,double *alpha,GIAC_CONTEXT) {
    start=0;
    if (g.type==_VECT && g.subtype!=_SEQ__VECT) {
        data=*g._VECTptr;
        len=data.size();
        return true;
    }
    if (g.type!=_VECT || g.subtype!=_SEQ__VECT || g._VECTptr->size()>3 || g._VECTptr->front().type!=_VECT)
        return false;
    vecteur &args=*g._VECTptr;
    data=*args.front()._VECTptr;
    len=data.size();
    bool has_alpha;
    if (is_real(args.at(1),contextptr)) {
        has_alpha=true;
        if (!alpha)
            return false;
        *alpha=_evalf(args.at(1),contextptr).DOUBLE_val();
    } else if (args.size()>2) return false;
    if (args.back().is_symb_of_sommet(at_interval)) {
        gen lh=_lhs(args.back(),contextptr),rh=_rhs(args.back(),contextptr);
        if (!lh.is_integer() || !rh.is_integer() || lh.val<0 || rh.val>=len)
            return false;
        start=lh.val;
        len=rh.val-start+1;
    } else if (args.size()>2 || !has_alpha) return false;
    return true;
}

gen _bartlett_hann_window(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    vecteur data;
    int start,N;
    identificateur k(" k");
    if (!parse_window_parameters(g,data,start,N,0,contextptr))
        return gentypeerr(contextptr);
    double a=0.62,b=0.48,c=0.38;
    gen expr=a-b*_abs(k/(N-1)-fraction(1,2),contextptr)-c*cos(2*k*_IDNT_pi()/(N-1),contextptr);
    return apply_window_function(expr,k,data,start,N,contextptr);
}
static const char _bartlett_hann_window_s []="bartlett_hann_window";
static define_unary_function_eval (__bartlett_hann_window,&_bartlett_hann_window,_bartlett_hann_window_s);
define_unary_function_ptr5(at_bartlett_hann_window,alias_at_bartlett_hann_window,&__bartlett_hann_window,0,true)

gen _blackman_harris_window(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    vecteur data;
    int start,N;
    identificateur k(" k");
    if (!parse_window_parameters(g,data,start,N,0,contextptr))
        return gentypeerr(contextptr);
    gen a(0.35875),b(0.48829),c(0.14128),d(0.01168);
    gen K=k*_IDNT_pi()/(N-1),expr=a-b*cos(2*K,contextptr)+c*cos(4*K,contextptr)-d*cos(6*K,contextptr);
    return apply_window_function(expr,k,data,start,N,contextptr);
}
static const char _blackman_harris_window_s []="blackman_harris_window";
static define_unary_function_eval (__blackman_harris_window,&_blackman_harris_window,_blackman_harris_window_s);
define_unary_function_ptr5(at_blackman_harris_window,alias_at_blackman_harris_window,&__blackman_harris_window,0,true)

gen _blackman_window(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    double alpha=0.16;
    vecteur data;
    int start,N;
    identificateur k(" k");
    if (!parse_window_parameters(g,data,start,N,&alpha,contextptr) || alpha<=0)
        return gentypeerr(contextptr);
    gen K=k*_IDNT_pi()/(N-1),expr=(1-alpha)/2-cos(2*K,contextptr)/2+alpha*cos(4*K,contextptr)/2;
    return apply_window_function(expr,k,data,start,N,contextptr);
}
static const char _blackman_window_s []="blackman_window";
static define_unary_function_eval (__blackman_window,&_blackman_window,_blackman_window_s);
define_unary_function_ptr5(at_blackman_window,alias_at_blackman_window,&__blackman_window,0,true)

gen _bohman_window(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    vecteur data;
    int start,N;
    identificateur k(" k");
    if (!parse_window_parameters(g,data,start,N,0,contextptr))
        return gentypeerr(contextptr);
    gen K=_abs(2*k/(N-1)-1,contextptr),expr=(1-K)*cos(_IDNT_pi()*K,contextptr)+sin(_IDNT_pi()*K,contextptr)/_IDNT_pi();
    return apply_window_function(expr,k,data,start,N,contextptr);
}
static const char _bohman_window_s []="bohman_window";
static define_unary_function_eval (__bohman_window,&_bohman_window,_bohman_window_s);
define_unary_function_ptr5(at_bohman_window,alias_at_bohman_window,&__bohman_window,0,true)

gen _cosine_window(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    double alpha=1.0;
    vecteur data;
    int start,N;
    identificateur k(" k");
    if (!parse_window_parameters(g,data,start,N,&alpha,contextptr) || alpha<=0)
        return gentypeerr(contextptr);
    gen expr=exp(alpha*ln(sin(k*_IDNT_pi()/(N-1),contextptr),contextptr),contextptr);
    return apply_window_function(expr,k,data,start,N,contextptr);
}
static const char _cosine_window_s []="cosine_window";
static define_unary_function_eval (__cosine_window,&_cosine_window,_cosine_window_s);
define_unary_function_ptr5(at_cosine_window,alias_at_cosine_window,&__cosine_window,0,true)

gen _gaussian_window(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    double alpha=0.1;
    vecteur data;
    int start,N;
    identificateur k(" k");
    if (!parse_window_parameters(g,data,start,N,&alpha,contextptr) || alpha<=0 || alpha>0.5)
        return gentypeerr(contextptr);
    gen c=(N-1)/2.0,expr=exp(-pow((k-c)/(alpha*c),2)/2,contextptr);
    return apply_window_function(expr,k,data,start,N,contextptr);
}
static const char _gaussian_window_s []="gaussian_window";
static define_unary_function_eval (__gaussian_window,&_gaussian_window,_gaussian_window_s);
define_unary_function_ptr5(at_gaussian_window,alias_at_gaussian_window,&__gaussian_window,0,true)

gen _hamming_window(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    vecteur data;
    int start,N;
    identificateur k(" k");
    if (!parse_window_parameters(g,data,start,N,0,contextptr))
        return gentypeerr(contextptr);
    gen a(0.54),b(0.46),expr=a-b*cos(2*_IDNT_pi()*k/(N-1),contextptr);
    return apply_window_function(expr,k,data,start,N,contextptr);
}
static const char _hamming_window_s []="hamming_window";
static define_unary_function_eval (__hamming_window,&_hamming_window,_hamming_window_s);
define_unary_function_ptr5(at_hamming_window,alias_at_hamming_window,&__hamming_window,0,true)

gen _hann_poisson_window(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    double alpha=1;
    vecteur data;
    int start,N;
    identificateur k(" k");
    if (!parse_window_parameters(g,data,start,N,&alpha,contextptr))
        return gentypeerr(contextptr);
    gen K=2*_IDNT_pi()*k/(N-1);
    gen expr=(1-cos(K,contextptr))*exp(-alpha*_abs(N-1-2*k,contextptr)/(N-1),contextptr)/2;
    return apply_window_function(expr,k,data,start,N,contextptr);
}
static const char _hann_poisson_window_s []="hann_poisson_window";
static define_unary_function_eval (__hann_poisson_window,&_hann_poisson_window,_hann_poisson_window_s);
define_unary_function_ptr5(at_hann_poisson_window,alias_at_hann_poisson_window,&__hann_poisson_window,0,true)

gen _hann_window(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    vecteur data;
    int start,N;
    identificateur k(" k");
    if (!parse_window_parameters(g,data,start,N,0,contextptr))
        return gentypeerr(contextptr);
    gen expr=pow(sin(_IDNT_pi()*k/(N-1),contextptr),2);
    return apply_window_function(expr,k,data,start,N,contextptr);
}
static const char _hann_window_s []="hann_window";
static define_unary_function_eval (__hann_window,&_hann_window,_hann_window_s);
define_unary_function_ptr5(at_hann_window,alias_at_hann_window,&__hann_window,0,true)

gen _parzen_window(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    vecteur data;
    int start,N;
    identificateur k(" k");
    if (!parse_window_parameters(g,data,start,N,0,contextptr))
        return gentypeerr(contextptr);
    gen K=1-2*k/(N-1),cond=symb_inferieur_egal(symbolic(at_abs,(N-1)/2.0-k),(N-1)/4.0);
    gen f1=1-6*pow(K,2)*(1-_abs(K,contextptr)),f2=2*pow(1-_abs(K,contextptr),3);
    gen expr=symbolic(at_when,makevecteur(cond,f1,f2));
    return apply_window_function(expr,k,data,start,N,contextptr);
}
static const char _parzen_window_s []="parzen_window";
static define_unary_function_eval (__parzen_window,&_parzen_window,_parzen_window_s);
define_unary_function_ptr5(at_parzen_window,alias_at_parzen_window,&__parzen_window,0,true)

gen _poisson_window(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    double alpha=1.0;
    vecteur data;
    int start,N;
    identificateur k(" k");
    if (!parse_window_parameters(g,data,start,N,&alpha,contextptr))
        return gentypeerr(contextptr);
    gen expr=exp(-alpha*_abs(2*k/(N-1)-1,contextptr),contextptr);
    return apply_window_function(expr,k,data,start,N,contextptr);
}
static const char _poisson_window_s []="poisson_window";
static define_unary_function_eval (__poisson_window,&_poisson_window,_poisson_window_s);
define_unary_function_ptr5(at_poisson_window,alias_at_poisson_window,&__poisson_window,0,true)

gen _riemann_window(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    vecteur data;
    int start,N;
    identificateur k(" k");
    if (!parse_window_parameters(g,data,start,N,0,contextptr))
        return gentypeerr(contextptr);
    gen K=(2*k/(N-1)-1)*_IDNT_pi(),cond=symbolic(at_same,makevecteur(k,(N-1)/2.0));
    gen expr=symbolic(at_when,makevecteur(cond,1,sin(K,contextptr)/K));
    return apply_window_function(expr,k,data,start,N,contextptr);
}
static const char _riemann_window_s []="riemann_window";
static define_unary_function_eval (__riemann_window,&_riemann_window,_riemann_window_s);
define_unary_function_ptr5(at_riemann_window,alias_at_riemann_window,&__riemann_window,0,true)

gen _triangle_window(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    double L=0;
    vecteur data;
    int start,N;
    identificateur k(" k");
    if (!parse_window_parameters(g,data,start,N,&L,contextptr) || (L!=1 && L!=-1 && L!=0))
        return gentypeerr(contextptr);
    gen expr=1-_abs((2*k-N+1)/(N+L),contextptr);
    return apply_window_function(expr,k,data,start,N,contextptr);
}
static const char _triangle_window_s []="triangle_window";
static define_unary_function_eval (__triangle_window,&_triangle_window,_triangle_window_s);
define_unary_function_ptr5(at_triangle_window,alias_at_triangle_window,&__triangle_window,0,true)

gen _tukey_window(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    double alpha=0.5;
    vecteur data;
    int start,N;
    identificateur k(" k");
    if (!parse_window_parameters(g,data,start,N,&alpha,contextptr) || alpha<0 || alpha>1)
        return gentypeerr(contextptr);
    double p=alpha*(N-1)/2.0,q=1-alpha/2;
    gen cond1=symb_inferieur_strict(k,p),cond2=symb_inferieur_egal(k,q*(N-1));
    gen f1=(1+cos(_IDNT_pi()*(k/p-1),contextptr))/2,f2=(1+cos(_IDNT_pi()*(k/p+1-2/alpha),contextptr))/2;
    gen expr=symbolic(at_piecewise,makevecteur(cond1,f1,cond2,1,f2));
    return apply_window_function(expr,k,data,start,N,contextptr);
}
static const char _tukey_window_s []="tukey_window";
static define_unary_function_eval (__tukey_window,&_tukey_window,_tukey_window_s);
define_unary_function_ptr5(at_tukey_window,alias_at_tukey_window,&__tukey_window,0,true)

gen _welch_window(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    vecteur data;
    int start,N;
    identificateur k(" k");
    if (!parse_window_parameters(g,data,start,N,0,contextptr))
        return gentypeerr(contextptr);
    double p=(N-1)/2.0;
    gen expr=1-pow(1-k/p,2);
    return apply_window_function(expr,k,data,start,N,contextptr);
}
static const char _welch_window_s []="welch_window";
static define_unary_function_eval (__welch_window,&_welch_window,_welch_window_s);
define_unary_function_ptr5(at_welch_window,alias_at_welch_window,&__welch_window,0,true)

#ifndef NO_NAMESPACE_GIAC
}
#endif // ndef NO_NAMESPACE_GIAC
