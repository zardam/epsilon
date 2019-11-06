function $id(id) { return document.getElementById(id); }

var UI = {
  Datestart:Date.now(),
  mailto: '',
  from: '',
  ready: false,
  focusaftereval: true,
  docprefix: "https://www-fourier.univ-grenoble-alpes.fr/%7eparisse/giac/doc/fr/cascmd_fr/",
  base_url: "https://www-fourier.univ-grenoble-alpes.fr/%7eparisse/",
  //forum_url: "http://xcas.e.univ-grenoble-alpes.fr/XCAS/viewforum.php?f=25",
  forum_url: "http://xcas.univ-grenoble-alpes.fr/forum/viewforum.php?f=25",
  // forum_url: "http://xcas.e.univ-grenoble-alpes.fr/XCAS/posting.php?mode=post&f=12&subject=session",
  //forum_url: "http://xcas.univ-grenoble-alpes.fr/forum/posting.php?mode=post&f=12&subject=session",
  forum_warn: true,
  focused: entree,
  savefocused: entree,
  usecm: true,
  fixeddel: false,
  kbdshift: false,
  usemathjax: false,
  prettyprint: true,
  qa: false,
  histcount: 0,
  selection: '',
  langue: -1,
  canvas_w: 350,
  canvas_h: 200,
  canvas_lastx: 0,
  canvas_lasty: 0,
  canvas_pushed: false,
  gr2d_ncanvas: 0,
  initconfigstring: '',
  python_mode: 0,
  python_indent: 4,
  warnpy: true, // set to false if you do not want Python compat warning
  xtn: 'x', // var name, depends on last app
  sleep: function (miliseconds) {
    var currentTime = new Date().getTime();
    while (currentTime + miliseconds >= new Date().getTime()) {
    }
  },
  is_touch_device: function () {
    return (('ontouchstart' in window)
        || (navigator.MaxTouchPoints > 0)
        || (navigator.msMaxTouchPoints > 0));
  },
  assistant_list: ['pour', 'tantque', 'solve', 'rsolve', 'fixe', 'test', 'prog', 'seq', 'plotpolar', 'series', 'limit', 'int', 'sum', 'diff', 'tabvarfunc', 'tabvarparam', 'plotfunc1var', 'plotfunc2var', 'plotparam', 'plotparam2var', 'plotimplicit', 'plotfield', 'desolve', 'matr', 'rand'
  ],
  assistant_close: function () {
    var t = UI.assistant_list;
    var s = t.length;
    for (var i = 0; i < s; i++) {
      $id('assistant_' + t[i]).style.display = 'none';
    }
  },
  assistant_ok: function () {
    var t = UI.assistant_list;
    var s = t.length;
    for (var i = 0; i < s; i++) {
      var tmp = $id('assistant_' + t[i]);
      if (tmp.style.display == 'block') {
        tmp = 'UI.assistant_' + t[i] + '_ok()';
        //console.log(tmp);
        eval(tmp);
        return true;
      }
    }
    return false;
  },
  assistant_pour_ok: function () {
    UI.focused = UI.savefocused;
    var st = $id('pourvarstep').value;
    if (UI.python_mode) {
      var sup=eval($id('pourvarmax').value)+1;
      UI.insert(UI.focused, '\nfor ' + $id('pourvarname').value + ' in range(' + $id('pourvarmin').value + ',' + sup);
      if (st.length) st = ',' + st;
      UI.insert(UI.focused, st + '):');
      UI.indentline(UI.focused);
      UI.insert(UI.focused, '\n');
      UI.indentline(UI.focused);
    }
    else {
      var tmp = '\npour ' + $id('pourvarname').value + ' de ' + $id('pourvarmin').value + ' jusque ' + $id('pourvarmax').value;
      //console.log(tmp);
      UI.insert(UI.focused, tmp);
      UI.indentline(UI.focused);
      if (st.length) UI.insert(UI.focused, ' pas ' + st);
      UI.insert(UI.focused, ' faire\n\nfpour;');
      UI.indentline(UI.focused);
      UI.moveCaretUpDown(UI.focused, -1);
      UI.indentline(UI.focused);
    }
    UI.focused.focus();
    $id('assistant_pour').style.display = 'none';
    $id('assistant_boucle').style.display = 'none';
  },
  assistant_tantque_ok: function () {
    UI.focused = UI.savefocused;
    if (UI.python_mode) {
      UI.insert(UI.focused, '\nwhile ' + $id('tantquecond').value + ':');
      UI.indentline(UI.focused);
      UI.insert(UI.focused, '\n');
    }
    else {
      UI.insert(UI.focused, '\ntantque ' + $id('tantquecond').value + ' faire\n\nftantque;');
      UI.indentline(UI.focused);
      UI.moveCaretUpDown(UI.focused, -1);
      UI.indentline(UI.focused);
      UI.moveCaretUpDown(UI.focused, -1);
      UI.indentline(UI.focused);
      UI.moveCaretUpDown(UI.focused, 1);
    }
    UI.focused.focus();
    $id('assistant_tantque').style.display = 'none';
    $id('assistant_boucle').style.display = 'none';
  },
  assistant_solve_ok: function () {
    UI.focused = UI.savefocused;
    if ($id('solveC').style.display == 'inline') UI.insert(UI.focused, 'c');
    if ($id('solvenum').style.display == 'inline') UI.insert(UI.focused, 'f');
    UI.insert(UI.focused, 'solve(' + $id('solveeq').value);
    UI.indentline(UI.focused);
    var tmp = $id('solvevar').value;
    if (tmp.length) UI.insert(UI.focused, ',' + tmp);
    UI.insert(UI.focused, ')');
    $id('assistant_solve').style.display = 'none';
  },
  assistant_rsolve_ok: function () {
    UI.focused = UI.savefocused;
    UI.insert(UI.focused, 'rsolve(' + $id('rsolveeq').value + ',' + $id('rsolvevar').value);
    var tmp = $id('rsolveinit').value;
    if (tmp.length) UI.insert(UI.focused, ',[' + tmp + ']');
    UI.insert(UI.focused, ')');
    $id('assistant_rsolve').style.display = 'none';
    $id('assistant_suites').style.display = 'none';
  },
  assistant_desolve_ok: function () {
    UI.focused = UI.savefocused;
    var tmpeq = $id('desolveeq').value;
    var tmpt = $id('desolvevar').value;
    var tmpy = $id('desolvey').value;
    var tmp = $id('desolveinit').value;
    if (tmp.length) tmp = 'desolve([' + tmpeq + ',' + tmp + ']'; else tmp = 'desolve(' + tmpeq;
    tmp += ',' + tmpt + ',' + tmpy + ')';
    UI.insert(UI.focused, tmp);
    $id('assistant_desolve').style.display = 'none';
  },
  assistant_fixe_ok: function () {
    UI.focused = UI.savefocused;
    var tmp = $id('rsolvef').value;
    if (tmp.length) {
      tmp += ':=' + $id('rsolvevarf').value + '->' + $id('rsolveexpr').value + ';\nplotseq(' + tmp + '(' + $id('rsolvevarf').value + ')';
    } else tmp = 'plotseq(' + $id('rsolveexpr').value;
    tmp += ',' + $id('rsolvevarf').value + '===';
    var tmp1 = $id('rsolvemin').value;
    if (tmp1.length) tmp += '[' + $id('rsolveu0').value + ',' + tmp1 + ',' + $id('rsolvemax').value + ']'; else tmp += $id('rsolveu0').value;
    tmp += ',' + $id('rsolven').value + ')';
    UI.insert(UI.focused, tmp);
    $id('assistant_fixe').style.display = 'none';
    $id('assistant_suites').style.display = 'none';
  },
  assistant_test_ok: function () {
    UI.focused = UI.savefocused;
    if (UI.python_mode) {
      UI.insert(UI.focused, '\nif ' + $id('sicond').value + ':');
      UI.indentline(UI.focused);
      UI.insert(UI.focused, '\n' + $id('sialors').value);
    } else UI.insert(UI.focused, '\nsi ' + $id('sicond').value + ' alors ' + $id('sialors').value);
    UI.indentline(UI.focused);
    var tmp = $id('sisinon').value;
    if (tmp.length) {
      if (UI.python_mode) {
        UI.indentline(UI.focused);
        UI.insert(UI.focused, '\nelse:')
        UI.indentline(UI.focused); // should remove 2 spaces at start
        UI.insert(UI.focused, '\n' + tmp);
      } else {
        UI.insert(UI.focused, ' sinon ' + tmp);
        UI.insert(UI.focused, ' fsi;\n');
      }
    }
    else {
      if (!UI.python_mode)
        UI.insert(UI.focused, ' fsi;\n');
    }
    UI.indentline(UI.focused);
    UI.funcoff();
    UI.focused.focus();
    $id('assistant_test').style.display = 'none';
  },
  assistant_prog_ok: function () {
    UI.focused = UI.savefocused;
    var loc = $id('localvars').value;
    var fc = $id('funcname').value;
    var argu = $id('argsname').value;
    var ret = $id('returnedvar').value;
    if (UI.python_mode) {
      UI.insert(UI.focused, 'def ' + fc + '(' + argu + '):');
      UI.indentline(UI.focused);
      if (0 && loc.length != 0) {
        UI.insert(UI.focused, '\n# local ' + loc);
        UI.indentline(UI.focused);
        UI.insert(UI.focused, '\n\nreturn ' + ret);
        UI.indentline(UI.focused);
        UI.moveCaretUpDown(UI.focused, -1);
        UI.indentline(UI.focused);
      }
      else {
        UI.insert(UI.focused, '\nreturn ' + ret);
        UI.indentline(UI.focused);
      }
    }
    else {
      if (loc.length == 0)
        UI.insert(UI.focused, 'fonction ' + fc + '(' + argu + ')\n \nffonction:;\n'); // was fc + '(' + argu + '):=' + ret + ';');
      else {
        UI.insert(UI.focused, 'fonction ' + fc + '(' + argu + ')\n  local ' + loc + ';\n  \n  retourne ' + ret + ';\nffonction:;\n');
        UI.moveCaretUpDown(UI.focused, -3);
        UI.moveCaret(UI.focused, 2);
      }
    }
    $id('assistant_prog').style.display = 'none';
    UI.focused.focus();
  },
  assistant_seq_ok: function () {
    UI.focused = UI.savefocused;
    var tmp = 'seq(' + $id('seqexpr').value + ',' + $id('seqvarname').value + ',' + $id('seqvarmin').value + ',' + $id('seqvarmax').value;
    UI.insert(UI.focused, tmp);
    tmp = $id('seqvarstep').value;
    if (tmp.length) UI.insert(UI.focused, ',' + tmp);
    UI.insert(UI.focused, ')');
    $id('assistant_seq').style.display = 'none';
  },
  assistant_plot_polar: function () {
    UI.focused = UI.savefocused;
    var tmp = 'plotpolar(' + $id('plotpolarexpr').value + ',' + $id('plotpolarvarname').value + ',' + $id('plotpolarvarmin').value + ',' + $id('plotpolarvarmax').value;
    UI.insert(UI.focused, tmp);
    tmp = $id('plotpolarvarstep').value;
    if (tmp.length) UI.insert(UI.focused, ',tstep===' + tmp);
    UI.insert(UI.focused, ')');
    $id('assistant_plotpolar').style.display = 'none';
  },
  assistant_series_ok: function () {
    UI.focused = UI.savefocused;
    var tmp = 'series(' + $id('seriesexpr').value + ',' + $id('seriesvarname').value + '===' + $id('seriesvarlim').value + ',' + $id('seriesvarorder').value;
    UI.insert(UI.focused, tmp);
    tmp = $id('seriesvarstep').value;
    if (tmp.length) UI.insert(UI.focused, ',' + tmp);
    UI.insert(UI.focused, ')');
    $id('assistant_series').style.display = 'none';
  },
  assistant_limit_ok: function () {
    UI.focused = UI.savefocused;
    var tmp = 'limit(' + $id('limitexpr').value + ',' + $id('limitvarname').value + ',' + $id('limitvarlim').value;
    UI.insert(UI.focused, tmp);
    tmp = $id('limitvardir').value;
    if (tmp.length) UI.insert(UI.focused, ',' + tmp);
    UI.insert(UI.focused, ')');
    $id('assistant_limit').style.display = 'none';
  },
  assistant_int_ok: function () {
    UI.focused = UI.savefocused;
    var tmp = $id('intexpr').value;
    if (tmp.length) {
      tmp = 'integrate(' + tmp + ',' + $id('intvarname').value;
      UI.insert(UI.focused, tmp);
      tmp = $id('intvarmin').value;
      if (tmp.length) UI.insert(UI.focused, ',' + tmp + ',' + $id('intvarmax').value);
      UI.insert(UI.focused, ')');
    } else UI.insert(UI.focused, 'integrate(');
    $id('assistant_int').style.display = 'none';
  },
  assistant_diff_ok: function () {
    UI.focused = UI.savefocused;
    var tmp = $id('diffexpr').value;
    if (tmp.length) {
      tmp = 'diff(' + tmp + ',' + $id('diffvarname').value;
      UI.insert(UI.focused, tmp);
      tmp = $id('diffnumber').value;
      if (tmp.length) UI.insert(UI.focused, ',' + tmp);
      UI.insert(UI.focused, ')');
    } else UI.insert(UI.focused, 'diff(');
    $id('assistant_diff').style.display = 'none';
  },
  assistant_sum_ok: function () {
    UI.focused = UI.savefocused;
    var tmp = $id('sumexpr').value;
    if (tmp.length) {
      tmp = 'sum(' + tmp + ',' + $id('sumvarname').value;
      UI.insert(UI.focused, tmp);
      tmp = $id('sumvarmin').value;
      if (tmp.length) UI.insert(UI.focused, ',' + tmp + ',' + $id('sumvarmax').value);
      UI.insert(UI.focused, ')');
    } else UI.insert(UI.focused, 'sum(');
    $id('assistant_sum').style.display = 'none';
  },
  assistant_tabvarfunc_ok: function () {
    UI.focused = UI.savefocused;
    $id('assistant_tabvar').style.display = 'none';
    $id('assistant_tabvarfunc').style.display = 'none';
    var tmp = $id('tabvarfuncname').value;
    if (tmp.length) {
      tmp += ':=' + $id('tabvarfuncvarname').value + '->' + $id('tabvarfuncexpr').value + ';\n';
      tmp = tmp + 'tabvar(' + $id('tabvarfuncname').value + '(' + $id('tabvarfuncvarname').value + '),';
    } else {
      tmp = tmp + 'tabvar(' + $id('tabvarfuncexpr').value + ','
    }
    tmp += $id('tabvarfuncvarname').value + ',' + $id('tabvarfuncvarmin').value + ',' + $id('tabvarfuncvarmax').value;
    var tmp1 = $id('tabvarfuncopt').value;
    if (tmp1.length) tmp += ',' + tmp1;
    tmp1 = $id('tabvarfuncvarstep').value;
    if (tmp1.length) tmp = tmp + ',xstep===' + tmp1;
    tmp += ',plot)';
    UI.insert(UI.focused, tmp);
  },
  assistant_tabvarparam_ok: function () {
    UI.focused = UI.savefocused;
    $id('assistant_tabvar').style.display = 'none';
    $id('assistant_tabvarparam').style.display = 'none';
    var varname = $id('tabvarparamvarname').value;
    var exprx = $id('tabvarparamexprx').value;
    var expry = $id('tabvarparamexpry').value;
    var tmp = $id('tabvarparamnamex').value;
    if (tmp.length) tmp = tmp + ':=' + varname + '->' + exprx + ';\n' + $id('tabvarparamnamey').value + ':=' + varname + '->' + expry + ';\n';
    tmp = tmp + 'tabvar([' + exprx + ',' + expry + '],' + varname + '===' + $id('tabvarparamvarmin').value + '..' + $id('tabvarparamvarmax').value;
    var tmp1 = $id('tabvarparamvarstep').value;
    if (tmp1.length) tmp = tmp + ',tstep===' + tmp1;
    tmp += ',plot)';
    UI.insert(UI.focused, tmp);
  },
  assistant_plotfunc1var_ok: function () {
    UI.focused = UI.savefocused;
    $id('assistant_plotfunc').style.display = 'none';
    $id('assistant_plotfunc1var').style.display = 'none';
    var tmp = 'plotfunc(' + $id('plotfuncexpr').value + ',' + $id('plotfuncvarname').value + ',' + $id('plotfuncvarmin').value + ',' + $id('plotfuncvarmax').value;
    var tmp1 = $id('plotfuncvarstep').value;
    if (tmp1.length) tmp = tmp + ',xstep===' + tmp1;
    tmp += ')';
    UI.insert(UI.focused, tmp);
  },
  assistant_plotfunc2var_ok: function () {
    UI.focused = UI.savefocused;
    $id('assistant_plotfunc').style.display = 'none';
    $id('assistant_plotfunc2var').style.display = 'none';
    var tmp = 'plotfunc(' + $id('plotfunc2expr').value + ',[' + $id('plotfunc2varx').value + ',' + $id('plotfunc2vary').value + ']';
    var tmp1 = $id('plotfunc2varxstep').value;
    if (tmp1.length) tmp = tmp + ',xstep===' + tmp1;
    tmp1 = $id('plotfunc2varystep').value;
    if (tmp1.length) tmp = tmp + ',ystep===' + tmp1;
    tmp += ')';
    UI.insert(UI.focused, tmp);
  },
  assistant_plotparam_ok: function () {
    UI.focused = UI.savefocused;
    $id('assistant_plotparam').style.display = 'none';
    $id('assistant_plotparam1var').style.display = 'none';
    var tmp = 'plotparam([' + $id('plotparamexprx').value + ',' + $id('plotparamexpry').value + '],' + $id('plotparamvarname').value + ',' + $id('plotparamvarmin').value + ',' + $id('plotparamvarmax').value;
    var tmp1 = $id('plotparamvarstep').value;
    if (tmp1.length) tmp = tmp + ',tstep===' + tmp1;
    tmp += ',display===cap_flat_line)';
    UI.insert(UI.focused, tmp);
  },
  assistant_plotparam2var_ok: function () {
    UI.focused = UI.savefocused;
    $id('assistant_plotparam').style.display = 'none';
    $id('assistant_plotparam2var').style.display = 'none';
    var tmp = 'plotparam([' + $id('plotparam2exprx').value + ',' + $id('plotparam2expry').value + ',' + $id('plotparam2exprz').value + '],[' + $id('plotparam2varx').value + ',' + $id('plotparam2vary').value + ']';
    var tmp1 = $id('plotparam2varxstep').value;
    if (tmp1.length) tmp = tmp + ',ustep===' + tmp1;
    tmp1 = $id('plotparam2varystep').value;
    if (tmp1.length) tmp = tmp + ',vstep===' + tmp1;
    tmp += ')';
    UI.insert(UI.focused, tmp);
  },
  assistant_plotimplicit_ok: function () {
    UI.focused = UI.savefocused;
    $id('assistant_plotimplicit').style.display = 'none';
    var ctr = $id('plotimplicitlevel').value;
    var tmp = $id('plotimplicitexprf').value + ',[' + $id('plotimplicitvarx').value + ',' + $id('plotimplicitvary').value + ']';
    if (ctr.length) tmp = 'plotcontour(' + tmp + ',' + ctr; else tmp = 'plotimplicit(' + tmp;
    var tmp1 = $id('plotimplicitvarxstep').value;
    if (tmp1.length) tmp = tmp + ',xstep===' + tmp1;
    tmp1 = $id('plotimplicitvarystep').value;
    if (tmp1.length) tmp = tmp + ',ystep===' + tmp1;
    tmp += ')';
    UI.insert(UI.focused, tmp);
  },
  assistant_plotfield_ok: function () {
    UI.focused = UI.savefocused;
    $id('assistant_plotfield').style.display = 'none';
    var ctr = $id('plotfieldinit').value;
    var tmp = $id('plotfieldexprf').value + ',[' + $id('plotfieldvarx').value + ',' + $id('plotfieldvary').value + ']';
    if (ctr.length) tmp = 'plotfield(' + tmp + ',plotode===' + ctr; else tmp = 'plotfield(' + tmp;
    var tmp1 = $id('plotfieldvarxstep').value;
    if (tmp1.length) tmp = tmp + ',xstep===' + tmp1;
    tmp1 = $id('plotfieldvarystep').value;
    if (tmp1.length) tmp = tmp + ',ystep===' + tmp1;
    tmp += ')';
    UI.insert(UI.focused, tmp);
  },
  assistant_plotpolar_ok: function () {
    UI.focused = UI.savefocused;
    var tmp = 'plotpolar(' + $id('plotpolarexpr').value + ',' + $id('plotpolarvarname').value + ',' + $id('plotpolarvarmin').value + ',' + $id('plotpolarvarmax').value;
    UI.insert(UI.focused, tmp);
    tmp = $id('plotpolarvarstep').value;
    if (tmp.length) UI.insert(UI.focused, ',tstep===' + tmp);
    UI.insert(UI.focused, ')');
    $id('assistant_plotpolar').style.display = 'none';
  },
  assistant_rand_setdisplay: function () {
    var form = $id('assistant_rand');
    if (form.rand_int.checked) {
      $id('chooselawdiv').style.display = 'none';
      $id('rand_intdiv').style.display = 'block';
    } else {
      $id('chooselawdiv').style.display = 'block';
      $id('law_arg').style.display = 'none';
      $id('rand_intdiv').style.display = 'none';
    }
  },
  assistant_matr_maxrows: 40,
  assistant_matr_maxcols: 6,
  assistant_matr_textarea: -1,
  assistant_matr_source: [], // JS array for spreadsheet, if length==0 matrix
  is_sheet: true,
  sheet_i: 0,
  sheet_j: 0,
  open_sheet: function (tableur) {
    UI.funcoff();
    UI.savefocused = UI.focused;
    $id('assistant_matr').style.display = 'block';
    $id('matr_type_chooser').style.display = 'inline';
    $id('matr_stats').style.display = 'none';
    UI.assistant_matr_setdisplay();
    UI.is_sheet = tableur;
    $id('matr_or_sheet').checked = tableur;
    $id('matr_type_chooser').style.display = tableur ? 'none' : 'inline';
    if (tableur) {
      $id('assistant_matr').matr_formuleshadow.checked = true;
      $id('assistant_matr').matr_formule.checked = false;
    }
    UI.assistant_matr_setdisplay();
    if (tableur) UI.matrix2spreadsheet(); else UI.spreadsheet2matrix(false);
    var field = $id('matr_span0_0');
    UI.sheet_onfocus(field);
    //UI.set_focus('matr_span0_0');
  },
  sheet_rowadd: function (n) {
    var field = $id('matr_nrows');
    field.value = eval(field.value) + n;
    var f = $id('stat_lmax');
    f.value++;
    UI.assistant_matr_setdisplay();
  },
  sheet_coladd: function (n) {
    var field = $id('matr_ncols');
    field.value = eval(field.value) + n;
    UI.assistant_matr_setdisplay();
  },
  sheet_edit_cmd: function (cmd) {
    var s = UI.focused.id;
    var sh = UI.is_sheet;
    if (!sh) {
      UI.is_sheet = true;
      UI.matrix2spreadsheet();
    }
    if (s === undefined) return;
    if (s.length < 12 || s.substr(0, 9) != 'matr_case') return;
    var l = s.length, i, j, err;
    for (i = 8; i < l; ++i) {
      if (s[i] == '_') break;
    }
    j = eval(s.substr(i + 1, l - i - 1));
    i = eval(s.substr(9, i - 9));
    s = cmd + ',' + i + ',' + j;
    //console.log(s);
    UI.sheet_recompute(s);
    if (!sh) {
      UI.is_sheet = false;
      UI.spreadsheet2matrix(false);
    }
    if (cmd == 2) UI.sheet_rowadd(1);
    if (cmd == 3) UI.sheet_rowadd(-1);
    if (cmd == 4) UI.sheet_coladd(1);
    if (cmd == 5) UI.sheet_coladd(-1);
  },
  sheet_recompute: function (cmd) {
    // if cmd=='' convert to CAS sheet, eval and convert back
    // else calls convert(matrix,cmd), where cmd='command,row,col',
    // command=0 copy down, =1 copy right
    // console.log('sheet_recompute',cmd);
    var R = UI.assistant_matr_maxrows;
    if (!UI.is_sheet || R == 0) return;
    var s = 'spreadsheet[';
    for (var i = 0; i < R; i++) {
      var C = UI.assistant_matr_maxcols;
      s += '[';
      for (var j = 0; j < C; j++) {
        var field = $id('matr_case' + i + '_' + j);
        var tmp = '[' + field.value;
        if (tmp.length == 1) tmp += '""';
        s += tmp + ',0,0],';
      }
      s += '],';
    }
    s += ']';
    if (cmd.length != 0)
      s = 'convert(' + s + ',cell,' + cmd + ')';
    //console.log(s);
    s = UI.caseval_noautosimp(s);
    if (s == ' Clic_on_Exec ') return;
    //console.log(s);
    s = eval(s);
    UI.assistant_matr_source = s;
    //console.log(s);
    if (R > s.length) R = s.length;
    // dispatch to UI
    for (var i = 0; i < R; i++) {
      var r = s[i];
      var C = r.length;
      if (C > UI.assistant_matr_maxcols) C = UI.assistant_matr_maxcols;
      for (var j = 0; j < C; j++) {
        var field = $id('matr_span' + i + '_' + j);
        var field2 = $id('matr_case' + i + '_' + j);
        //console.log(i,j,field,r);
        var tmp = r[j], tmp2;
        if (tmp === undefined) continue;
        if (r[j].length > 1) {
          tmp2 = tmp[0];
          tmp = tmp[1];
        } else tmp2 = tmp = tmp[0];
        if (r[j].length == 3) tmp = r[j][2];
        // console.log(i,j,tmp);
        if (tmp == '""') tmp = '&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;';
        field.innerHTML = tmp;
        if (tmp2 == '""') tmp2 = '';
        field2.value = tmp2;
        //console.log(i,j,tmp,field.innerHTML);
        // field.style.display='inline';
      }
    }
    return 1;
  },
  matrix2spreadsheet: function () {
    // convert matrix headers to spreadsheet and store source formulae
    var l = UI.assistant_matr_maxrows;
    var c = UI.assistant_matr_maxcols;
    //console.log(l,c);
    if (c > 26) {
      UI.assistant_matr_maxcols = c = 26;
    }
    for (var j = 0; j < c; j++) {
      $id('matr_head_' + j).innerHTML = String.fromCharCode(65 + j);
    }
    UI.is_sheet = true;
    UI.sheet_recompute('');
    for (var i = 0; i < l; i++) {
      for (var j = 0; j < c; j++) {
        //console.log(i,j);
        var field = $id('matr_span' + i + '_' + j);
        field.style.display = 'inline';
        field.previousSibling.style.display = 'none';
      }
    }
  },
  spreadsheet2matrix: function (evaled) {
    var l = UI.assistant_matr_maxrows;
    var c = UI.assistant_matr_maxcols;
    if (c > 26) {
      UI.assistant_matr_maxcols = c = 26;
    }
    for (var j = 0; j < c; j++) {
      $id('matr_head_' + j).innerHTML = '' + j;
    }
    for (var i = 0; i < l; i++) {
      ligne = UI.assistant_matr_source[i];
      for (var j = 0; j < c; j++) {
        var tmp = ligne[j][1];
        if (tmp == '""') tmp = '';
        var field = $id('matr_case' + i + '_' + j);
        if (evaled) field.value = tmp;
        field.style.display = 'inline';
        field.nextSibling.style.display = 'none';
      }
    }
    UI.is_sheet = false;
  },
  sheet_onfocus: function (field) {
    //console.log(field.id,field.previousSibling.id);
    s = field.id;
    var l = s.length, i, j, err;
    for (i = 8; i < l; ++i) {
      if (s[i] == '_') break;
    }
    j = eval(s.substr(i + 1, l - i - 1));
    i = eval(s.substr(9, i - 9));
    $id('matr_line_' + UI.sheet_i).style.color = "black";
    $id('matr_head_' + UI.sheet_j).style.color = "black";
    UI.sheet_i = i;
    UI.sheet_j = j;
    $id('matr_line_' + UI.sheet_i).style.color = "red";
    $id('matr_head_' + UI.sheet_j).style.color = "red";
    field = field.previousSibling;
    field.style.display = 'inline';
    field.focus();
    UI.focused = field;
  },
  sheet_set: function (field, value) {
    field.value = value;
    var s = field.id;
    if (s.length > 9 && s.substr(0, 9) == 'matr_case')
      $id('matr_span' + s.substr(9, s.length - 9)).innerHTML = value;
  },
  assistant_matr_setmatrix: function (l, c) {
    // Code de creation de la matrice
    if (l * c > 10000) {
      l = 100;
      c = 100;
    }
    var mydiv = $id('matr_casediv');
    mydiv.style.maxHeight = window.innerHeight / 2 + 'px';
    mydiv.style.maxWidth = (window.innerWidth - 50) + 'px';
    mydiv.style.overflow = "auto";
    UI.assistant_matr_maxrows = l;
    UI.assistant_matr_maxcols = c;
    var s = '<table>\n';
    var h = '<tr><th id="matr_head">@</th>';
    for (var j = 0; j < c; ++j) {
      h += '<th id="matr_head_' + j + '" style="text-align:center">' + j + '</th>';
    }
    h += '</tr>\n';
    s += h;
    for (var i = 0; i < l; ++i) {
      s += '<tr id="matr_ligne' + i + '">';
      s += '<td id="matr_line_' + i + '">' + i + '</td>';
      for (var j = 0; j < c; ++j) {
        var field = $id('matr_case' + i + '_' + j);
        var oldval = '';
        //console.log(i,j,field.value);
        if (field !== null) oldval = field.value;
        if (UI.assistant_matr_textarea > 0)
          s += '<td class="matrixcell"><textarea class="matrixcell" \
onkeypress="if (event.keyCode!=13) return true; UI.cb_matr_enter(this,true); return false;" \
onclick="UI.focused=this;" onblur="UI.sheet_blur(this)" onfocus="nextSibling.style.display=\'none\';UI.focused=this;" \
id="matr_case' + i + '_' + j + '">' + oldval + '</textarea><div class="matrixcell" style="display:none;width:20px" onclick="UI.sheet_onfocus(this);" id="matr_span' + i + '_' + j + '"></div></td>';
        else
          s += '<td class="matrixcell" onclick="UI.sheet_onfocus(lastChild);"><input class="matrixcell" onkeypress="if (event.keyCode!=13) return true; UI.cb_matr_enter(this,true); return false;" onclick="UI.focused=this;" onblur="UI.sheet_blur(this)" onfocus="nextSibling.style.display=\'none\';UI.focused=this;" id="matr_case' + i + '_' + j + '" value="' + oldval + '" /><div class="matrixcell" style="display:none;width:20px" onclick="UI.sheet_onfocus(this);"  id="matr_span' + i + '_' + j + '"></div></td>';
      }
      s += '</tr>\n';
    }
    s += '</table>';
    //console.log(s);
    mydiv.innerHTML = s;
    if ($id('matr_or_sheet').checked) {
      UI.matrix2spreadsheet();
    }
    //console.log(mydiv.innerHTML);
    //mydiv.style.display='none';
  },
  sheet_blur: function (field) {
    UI.cb_matr_enter(field, false);
    //console.log(field.innerHTML);
    //console.log(field.nextSibling.innerHTML);
    field.style.display = 'none';
    field.nextSibling.style.display = 'inline';
  },
  cb_matr_enter: function (field, focusnext) {
    var s = 'csv2gen("' + field.value + '",string)';
    //console.log(s);
    var se = UI.caseval_noautosimp(s);
    if (se.length > 8 && se.substr(0, 8) == 'matrix[[')
      se = se.substr(6, se.length - 6);
    //console.log(se);
    s = field.id;
    var l = s.length, i, j, err;
    for (i = 8; i < l; ++i) {
      if (s[i] == '_') break;
    }
    j = eval(s.substr(i + 1, l - i - 1));
    i = eval(s.substr(9, i - 9));
    var form = $id('assistant_matr'), nr = form.matr_nrows.value, nc = form.matr_ncols.value;
    try {
      se = eval(se);
    } catch (err) {
      se = UI.caseval_noautosimp(field.value);
    }
    // console.log(i,j); // position dans la matrice
    if (!Array.isArray(se)) {
      //console.log(i,j,field.id); // position dans la matrice
      UI.sheet_set(field, se);
      i++;
      if (i >= nr) {
        i = 0;
        j++;
        if (j >= nc) j = 0;
      }
      UI.sheet_recompute('');
      var tmp = 'matr_span' + i + '_' + j; // console.log(tmp);
      if (focusnext) $id(tmp).click();
      return;
    }
    else {
      l = se.length;
      if (l == 0) return;
      var cell = se[0];
      if (Array.isArray(cell)) {
        // remplissage style matrice
        var iend = i + l, j0 = j, ishift = i, extend = false;
        if (iend >= UI.assistant_matr_maxnrows) iend = UI.assistant_matr_maxnrows;
        if (iend > form.matr_nrows.value) {
          extend = true;
          form.matr_nrows.value = iend;
        }
        for (; i < iend; ++i) {
          var ligne = se[i - ishift];
          if (Array.isArray(ligne)) {
            var j_ = j, jend = j + ligne.length;
            if (jend >= UI.assistant_matr_maxncols) jend = UI.assistant_matr_maxncols;
            if (jend > form.matr_ncols.value) {
              extend = true;
              form.matr_ncols.value = jend;
            }
            for (; j_ < jend; ++j_) UI.sheet_set($id('matr_case' + i + '_' + j_), ligne[j_ - j]);
          }
          else
            UI.sheet_set($id('matr_case' + i + '_' + j), ligne);
        }
        if (extend) {
          UI.assistant_matr_setdisplay();
          UI.sheet_recompute('');
        }
        else {
          //console.log(iend,j0);
          if (iend >= nr) iend = 0;
          if (j0 >= nc) j0 = 0;
          UI.sheet_recompute('');
          tmp = 'matr_span' + iend + '_' + j0;
          if (focusnext) $id(tmp).click();
        }
        return;
      }
      // remplissage ligne i, colonnes j -> j+l-1
      var jend = j + l, shift = j;
      if (jend >= UI.assistant_matr_maxncols) jend = UI.assistant_matr_maxncols;
      for (; j < jend; ++j)
        UI.sheet_set($id('matr_case' + i + '_' + j), se[j - shift]);
      UI.sheet_recompute('');
    }
  },
  adequation: function (form) {
    var test;
    for (test = 0; test < 7; ++test) {
      if (form.adequation[test].checked) break;
    }
    //console.log(test);
    if (test == 0 || test == 1) {
      $id('matr').style.display = 'none';
      $id('matr_matr').style.display = 'none';
    }
    else {
      $id('matr').style.display = 'block';
      $id('matr_matr').style.display = 'block';
    }
  },
  assistant_rand_ok: function () {
    $id('assistant_rand').style.display = 'none';
    $id('chooselawdiv').style.display = 'none';
    UI.focused = UI.savefocused;
    var nr = $id('rand_nrows').value;
    var nc = $id('rand_ncols').value, tmp;
    if (nc <= 0)
      tmp = 'rand(';
    else {
      if (nr <= 0)
        tmp = 'ranv(' + nc + ',';
      else
        tmp = 'ranm(' + nr + ',' + nc + ',';
    }
    var form = $id('assistant_rand');
    var entier = form.rand_int.checked;
    if (entier) {
      var nm = $id('rand_maxint').value;
      tmp += nm + ')';
    }
    else {
      form = $id('chooselawform');
      tmp += form.rand_law.value;
      if (form.rand_law1.style.display != 'none')
        tmp += ',' + form.rand_law1.value;
      if (form.rand_law2.style.display != 'none')
        tmp += ',' + form.rand_law2.value;
      tmp += ')';
    }
    //console.log(tmp);
    UI.insert(UI.focused, tmp);
  },
  assistant_matr_ok: function () {
    var stats = $id('matr_stats').style.display != 'none';
    var stat12 = $id('matr_stat12').style.display != 'none';
    var stathyp = $id('matr_testhyp').style.display != 'none';
    var fluctu = $id('risque_alpha').style.display != 'none';
    $id('assistant_matr').style.display = 'none';
    $id('chooselawdiv').style.display = 'none';
    $id('risque_alpha').style.display = 'none';
    UI.focused = UI.savefocused;
    var tmp;
    if (stats && !stat12 && !stathyp) {
      var form = $id('chooselawform');
      tmp = form.rand_law.value;
      if (fluctu) tmp += '_icdf';
      tmp += '(';
      if (form.rand_law1.style.display != 'none')
        tmp += form.rand_law1.value + ',';
      if (form.rand_law2.style.display != 'none')
        tmp += form.rand_law2.value + ',';
      if (fluctu) {
        var alpha = 0.05;
        var alphaf = $id('adequation_alpha');
        //console.log(alphaf.value); return;
        if (alphaf.value.length) alpha = eval(alphaf.value);
        tmp = tmp + alpha / 2 + '),\n' + tmp + (1 - alpha / 2) + ');';
      }
      else {
        var argu = $id('law_arg');
        tmp += argu.value + ')';
      }
      UI.insert(UI.focused, tmp);
      return;
    }
    var form = $id('assistant_matr');
    var mat = form.matr_name.value;
    if (stats && mat.length == 0) mat = 'm_s';
    var nrows = $id('matr_nrows');
    var ncols = $id('matr_ncols');
    var maxrows = 40, maxcols = 6;
    if (ncols.value > maxcols) ncols.value = maxcols;
    if (nrows.value > maxrows) nrows.value = maxrows;
    if (mat.length) tmp = mat + ':=';
    if (form.matr_formule.checked) {
      tmp += 'matrix(' + nrows.value + ',' + ncols.value + ',';
      tmp += '(j,k)->';
      var expr = $id('matr_expr').value;
      //console.log(expr);
      if (!form.matr_start0.checked) expr = UI.caseval_noautosimp('subst(' + expr + ',[j,k],[j+1,k+1])');
      tmp += expr;
      tmp += ')';
    }
    else {
      tmp += '[';
      for (var i = 0; i < nrows.value; i++) {
        tmp += '[';
        for (var j = 0; j < ncols.value; j++) {
          //console.log(j,tmp);
          var val = $id('matr_case' + i + '_' + j).value;
          if (UI.is_sheet && i < UI.assistant_matr_source.length) {
            var ligne = UI.assistant_matr_source[i];
            if (j < ligne.length) {
              ligne = ligne[j];
              if (ligne.length > 1) {
                ligne = ligne[1];
                if (ligne != '""') val = ligne;
              }
            }
          }
          if (val.length > 0) tmp += val; else tmp += '0';
          if (j < ncols.value - 1) tmp += ',';
        }
        tmp += ']';
        if (i < nrows.value - 1) tmp += ',\n';
      }
      tmp += ']';
    }
    if (stats) {
      tmp += ':;\n';
      var submat = mat + '[' + form.stat_lmin.value + '..' + form.stat_lmax.value + ',' + form.stat_cmin.value + '..' + form.stat_cmax.value + ']';
      if (stat12) {
        if (form.stat_mean.checked) tmp += 'mean(' + submat + ');\n';
        if (form.stat_stddev.checked) tmp += 'stddev(' + submat + ');\n';
        if (form.stat_quartiles.checked) tmp += 'quartiles(' + submat + ');\n';
        if (form.stat_histo.checked) tmp += 'histogram(' + submat + ',0,1);\n';
        if (form.stat_moustache.checked) tmp += 'moustache(' + submat + ');\n';
        if (form.stat_scatter.checked) tmp += 'scatterplot(' + submat + ');\n';
        if (form.stat_polygonscatter.checked) tmp += 'polygonscatterplot(' + submat + ');\n';
        if (form.stat_linreg.checked) tmp += 'linear_regression_plot(' + submat + ');\n';
      }
      else {
        var alpha = 0.05, test, hyp, mu = form.adequation_mu.value, sigma = 0, dof = 1;
        if (form.adequation_alpha.value.length)
          alpha = form.adequation_alpha.value;
        if (form.adequation_sigma.value.length)
          sigma = form.adequation_sigma.value;
        for (test = 0; test < 7; ++test) {
          if (form.adequation[test].checked) break;
        }
        for (hyp = 0; hyp < 3; ++hyp) {
          if (form.adequation_alt[hyp].checked) break;
        }
        if (test == 3) tmp += 'chisquaret(';
        if (test == 4) tmp += 'kolmogorovt(';
        if (test == 5) tmp += 'wilcoxont(';
        var l1 = form.stat_lmin.value, l2 = form.stat_lmax.value,
            c1 = form.stat_cmin.value, c2 = form.stat_cmax.value;
        if (test >= 3 && test <= 5) {
          if (l2 - l1 > c2 - c1) { // lines
            tmp += mat + '[' + l1 + '..' + l2 + ',' + c1 + '],';
            tmp += mat + '[' + l1 + '..' + l2 + ',' + c2 + ']';
            dof = l2 - l1;
          }
          else {
            tmp += mat + '[' + l1 + ',' + c1 + '..' + c2 + '],';
            tmp += mat + '[' + l2 + ',' + c1 + '..' + c2 + ']';
            dof = c2 - c1;
          }
          if (test == 5) {
            if (hyp == 0) tmp += ',\'<\'';
            if (hyp == 1) tmp += ',\'!=\'';
            if (hyp == 2) tmp += ',\'>\'';
            tmp += ',' + alpha;
          }
          tmp += ');';
        }
        if (test == 3) tmp += 'chisquare_icdf(' + dof + ',' + (1 - alpha) + ');';
        if (test == 6) tmp += 'normalt(';
        if (test == 7) tmp += 'studentt(';
        if (test == 0) {
          var n = form.confiance_n.value, p = form.confiance_p.value;
          var coeff = '1/2';
          if (n > 100) coeff = 'sqrt(' + (n / (n - 1) * p * (1 - p)) + ')';
          tmp = 'p:=' + p + ';n:=' + n + ';alpha:=' + alpha + ';\ndelta_p:=' + coeff + '*normald_icdf(0,1,(1-alpha/2))/sqrt(n);';
          tmp += '\n[p-delta_p,p+delta_p];\n';
          if (n * p < 5 || n * (1 - p) < 5) tmp = "Erreur : n*p et n*(1-p) doivent etre plus grand que 5";
          UI.insert(UI.focused, tmp);
          return;
        }
        if (test == 1) {
          var n = form.confiance_n_.value, mu = form.confiance_mu.value, sigma = form.confiance_sigma.value;
          tmp = 'mu:=' + mu + '; sigma:=' + sigma + ';\n';
          tmp += 'delta:=sigma/sqrt(' + n + ')*student_icdf(' + n + ',' + (1 - alpha / 2) + ');\n';
          tmp += '[mu-delta,mu+delta]';
        }
        if (test == 2) {
          tmp += 'mu:=mean(flatten(' + submat + '));\n'
          dof = (c2 - c1 + 1) * (l2 - l1 + 1) - 1;
          tmp += 'delta:=stddevp(flatten(' + submat + '))/sqrt(' + dof + ')*student_icdf(' + dof + ',' + (1 - alpha / 2) + ');\n';
          tmp += '[mu-delta,mu+delta]';
        }
        if (test == 6 || test == 7) {
          tmp += 'flatten(' + submat + '),' + mu + ',';
          if (sigma > 0)
            tmp += sigma + ',';
          if (hyp == 0) tmp += '\'<\'';
          if (hyp == 1) tmp += '\'!=\'';
          if (hyp == 2) tmp += '\'>\'';
          tmp += ',' + alpha + ')';
        }
      }
    }
    //console.log(tmp);
    UI.insert(UI.focused, tmp);
  },
  assistant_matr_setdisplay: function () {
    $id('matr_matr').style.display = 'block';
    $id('chooselawdiv').style.display = 'none';
    var form = $id('assistant_matr');
    if (form.matr_nrows.value < 0) form.matr_nrows.value = 0;
    if (form.matr_ncols.value < 0) form.matr_ncols.value = 0;
    if ($id('matr_stats').style.display != 'none') {
      if (form.matr_nrows.value < 1) form.matr_nrows.value = 1;
      if (form.matr_ncols.value < 1) form.matr_ncols.value = 1;
      if (form.stat_lmax.value >= form.matr_nrows.value)
        form.stat_lmax.value = form.matr_nrows.value - 1;
      if (form.stat_lmin.value >= form.matr_nrows.value)
        form.stat_lmin.value = form.matr_nrows.value - 1;
      form.stat_lmax.max = form.matr_nrows.value - 1;
      form.stat_lmin.max = form.matr_nrows.value - 1;
      if (form.stat_cmax.value >= form.matr_ncols.value)
        form.stat_cmax.value = form.matr_ncols.value - 1;
      if (form.stat_cmin.value >= form.matr_ncols.value)
        form.stat_cmin.value = form.matr_ncols.value - 1;
      form.stat_cmax.max = form.matr_ncols.value - 1;
      form.stat_cmin.max = form.matr_ncols.value - 1;
    }
    form.matr_ncols.max = UI.assistant_matr_maxncols;
    form.matr_nrows.max = UI.assistant_matr_maxnrows;
    if (form.matr_formule.checked) {
      $id('matr_formulediv').style.display = 'inline';
      UI.set_focus('matr_expr');
      $id('matr').style.display = 'none';
    } else {
      $id('matr_formulediv').style.display = 'none';
      $id('matr').style.display = 'block';
      var nrows = $id('matr_nrows');
      var ncols = $id('matr_ncols');
      var maxrows = UI.assistant_matr_maxrows, maxcols = UI.assistant_matr_maxcols;
      //console.log(nrows.value,maxrows,ncols.value,maxcols);
      if (ncols.value < 1) ncols.value = 1;
      if (ncols.value > maxcols) ncols.value = maxcols;
      if (nrows.value < 1) nrows.value = 1;
      if (nrows.value > maxrows) nrows.value = maxrows;
      //console.log("matr",nrows.value,ncols.value);
      var matr = [];
      matr.length = maxrows;
      for (var j = 0; j < maxcols; j++) {
        var tmp = $id('matr_head_' + j);
        if (j < ncols.value) tmp.style.visibility = 'visible'; else tmp.style.visibility = 'hidden';
      }
      for (var i = 0; i < maxrows; i++) {
        //$id('matr_ligne'+i).style.visibility='hidden';
        $id('matr_ligne' + i).style.display = 'none';
        var ligne = [];
        ligne.length = maxcols;
        for (var j = 0; j < maxcols; j++) {
          ligne[j] = $id('matr_case' + i + '_' + j);
          ligne[j].style.display = 'none';
          //console.log(j,ligne[j].parentNode.style.display);
          ligne[j].parentNode.style.display = 'none';
          ligne[j].nextSibling.style.display = 'none';
        }
        matr[i] = ligne;
      }
      for (var i = 0; i < nrows.value; i++) {
        //$id('matr_ligne'+i).style.visibility='visible';
        $id('matr_ligne' + i).style.display = 'table-row';
        if (UI.is_sheet) {
          for (var j = 0; j < ncols.value; j++) {
            var f = matr[i][j];
            // console.log(i,j,f.innerHTML);
            f.nextSibling.style.display = 'inline';
            f.parentNode.style.display = '';
          }
        }
        else {
          for (var j = 0; j < ncols.value; j++) {
            matr[i][j].style.display = 'inline';
            matr[i][j].parentNode.style.display = '';
          }
        }
      }
      //UI.set_focus(matr[0][0]);
    }
  },
  toggleshift: function () {
    UI.kbdshift = !UI.kbdshift;
    if (UI.kbdshift) {
      $id('shift_key').style.backgroundColor = "white";
      $id('add_left_par').value = "[";
      $id('add_right_par').value = "]";
      $id('add_ln').value = "ln";
      $id('add_sin').value = "asin";
      $id('add_cos').value = "acos";
      $id('add_tan').value = "atan";
      $id('add_newline').value = "\\n";
      $id('add_,').value = "'";
      $id('add_:').value = "!";
      $id('add_semi').value = "\"";
      $id('add-=').value = "_";
      // $id('add_infer').value=">";
      $id('add_sqrt').value = "^2";
      $id('add_/').value = "%";
      $id('add_pi').value = "∞";
    }
    else {
      $id('shift_key').style.backgroundColor = "cyan";
      $id('add_left_par').value = "(";
      $id('add_right_par').value = ")";
      $id('add_ln').value = "exp";
      $id('add_sin').value = "sin";
      $id('add_cos').value = "cos";
      $id('add_tan').value = "tan";
      $id('add_newline').value = " ";
      $id('add_,').value = ",";
      $id('add_:').value = UI.python_mode ? ':' : ':=';
      $id('add_semi').value = ";";
      $id('add-=').value = "=";
      // $id('add_infer').value="<";
      $id('add_sqrt').value = "√";
      $id('add_/').value = "/";
      $id('add_pi').value = "π";
    }
  },
  codemirror_setoptions: function (cmentree) {
    UI.setoption_mode(cmentree);
    cmentree.on("focus", function (cm) {
      UI.set_focused(cm);
      UI.set_editline(cm, true); // insure OK is visible on mobile
      UI.set_config_width();
    });
    cmentree.setOption("extraKeys", {
      Enter: function (cm) { // guess if newline evaluates or adds a newline
        var start = cm.getCursor('from');
        var end = cm.getCursor('to');
        var tst = cm.lineCount() > 1;
        if (!tst) { // if the line begins by function/fonction or def/for/if/while/si/tantque/pour
          var txt = cm.getLine(end.line);
          //console.log(txt);
          var l = txt.length, i, j;
          for (i = 0; i < l; i++) {
            if (txt[i] != ' ') break;
          }
          for (j = i; j < l; j++) {
            if (txt[j] == ' ') break;
          }
          txt = txt.substr(i, j - i);
          //console.log(txt);
          if (txt == "for" || txt == "while" || txt == "if" || txt == "pour" || txt == "tantque" || txt == "si" || txt == "def" || txt == "fonction" || txt == "function")
            tst = true;
        }
        if (tst && (end.line != start.line || end.ch != start.ch || ((start.line > 0 || start.ch > 0) && UI.not_empty(cm.getLine(end.line)))))
          UI.insert(cm, '\n');
        else
        //UI.set_editline(cmentree,false);
          UI.eval_cmdline();
      },
      "Ctrl-Enter": function (cm) {
        //UI.set_editline(cmentree,false);
        UI.eval_cmdline();
      },
      Backspace: function (cm) {
        UI.backspace(cm);
      },
      F1: function (cm) {
        UI.completion(cm);
      },
      "Ctrl-Space": function (cm) {
        UI.completion(cm);
      },
      Tab: function (cm) {
        UI.indent_or_complete(cm);
      },
    });
  },
  switchcm: function () {
    if (UI.usecm) {
      if (cmentree == entree) {
        // cmentree may be released with cmentree.toTextArea();
        cmentree = CodeMirror.fromTextArea(entree, {
          matchBrackets: true,
          lineNumbers: true,
          viewportMargin: Infinity
        });
        UI.setoption_mode(cmentree);
        //console.log(entree.type);
        //cmentree.setSize(window.innerWidth-20,40);
        cmentree.options.indentUnit = UI.python_mode ? UI.python_indent : 2;
        cmentree.on("focus", function (cm) {
          UI.set_focused(cm);
          UI.set_config_width();
        });
        cmentree.on("blur", function (cm) {
          if (cm.getSelection().length > 0) {
            UI.selection = cm.getSelection();
          }
        });
        cmentree.setValue(entree.value);
        UI.changefontsize(cmentree, 18);
        UI.codemirror_setoptions(cmentree);
      } // if (UI.usecm)
    } else {
      if (cmentree != entree) cmentree.toTextArea();
      cmentree = entree;
    }
    cmentree.focus();
  },
  setoption_mode: function (cmentree) {
    if (!UI.usecm) return;
    if (UI.python_mode) {
      //console.log('Python mode');
      cmentree.setOption("mode", "python");
    }
    else {
      //console.log('Xcas mode');
      cmentree.setOption("mode", "simplemode");
    }
  },
  kbdonfuncoff: function () {
    UI.savefocused = UI.focused;
    $id('keyboard').style.display = 'inline';
    $id('keyboardfunc').style.display = 'none';
  },
  funcoff: function () {
    UI.savefocused = UI.focused;
    $id('keyboardfunc').style.display = 'none';
  },
  restorefrom: function (c) {
    var s = UI.readCookie(c);
    UI.restoresession(s, $id('mathoutput'), true, false);
    //console.log(c.substring(6,c.length));
    $id('outputfilename').value = c.substring(6, c.length);
    $id('loadfile_cookie').innerHTML = '';
  },
  listCookies: function () { // list cookies with name begin == ' xcas__'
    var aString = '';
    if (window.localStorage) {
      for (var i = 0, len = localStorage.length; i < len; i++) {
        var tmp = localStorage.key(i);
        //console.log(tmp);
        if (tmp.substr(0, 6) == 'xcas__') {
          var tmpname = tmp.substr(6, tmp.length - 6);
          aString += "<button onclick=\"UI.restorefrom('" + tmp.substr(0, pos) + "')\">" + tmpname + "</button>\n";
        }
      }
    }
    var theCookies = document.cookie.split(';');
    for (var i = 0; i < theCookies.length; i++) {
      // console.log(i,theCookies[i].substr(0,7));
      var tmp = theCookies[i];
      var pos = tmp.search('=');
      if (pos > 7 && tmp.substr(0, 7) == ' xcas__') {
        var tmpname = tmp.substr(7, pos - 7);
        aString += "<button onclick=\"UI.restorefrom('" + tmp.substr(1, pos - 1) + "')\">" + tmpname + "</button>\n";
      }
    }
    aString += "<button onclick=$id('loadfile_cookie').innerHTML=''>X</button>\n"
    //console.log(aString);
    return aString;
  },
  createCookie: function (name, value, days) {
    if (window.localStorage) return localStorage.setItem(name, value);
    if (days) {
      var date = new Date();
      date.setTime(date.getTime() + (days * 24 * 60 * 60 * 1000));
      var expires = "; expires=" + date.toGMTString();
    }
    else var expires = "";
    document.cookie = name + "=" + value + expires + "; path=/";
  },
  readCookie: function (name) {
    if (window.localStorage) {
      var tmp = localStorage.getItem(name);
      if (tmp != null) return tmp;
    }
    var nameEQ = name + "=";
    var ca = document.cookie.split(';');
    for (var i = 0; i < ca.length; i++) {
      var c = ca[i];
      while (c.charAt(0) == ' ') c = c.substring(1, c.length);
      if (c.indexOf(nameEQ) == 0) return c.substring(nameEQ.length, c.length);
    }
    return null;
  },
  eraseCookie: function (name) {
    if (window.localStorage) return localStorage.removeItem(name);
    createCookie(name, "", -1);
  },
  detectmob: function () {
    if (navigator.userAgent.match(/Android/i)
        || navigator.userAgent.match(/webOS/i)
        || navigator.userAgent.match(/iPhone/i)
        || navigator.userAgent.match(/iPad/i)
        || navigator.userAgent.match(/iPod/i)
        || navigator.userAgent.match(/BlackBerry/i)
        || navigator.userAgent.match(/Windows Phone/i)
    ) return true;
    else
      return false;
  },
  browser_type: function () {
    var isOpera = !!window.opera || navigator.userAgent.indexOf(' OPR/') >= 0;
    var isFirefox = typeof InstallTrigger !== 'undefined';   // Firefox 1.0+
    var isSafari = Object.prototype.toString.call(window.HTMLElement).indexOf('Constructor') > 0;
    var isChrome = !!window.chrome && !isOpera;              // Chrome 1+
    var isIE = /*@cc_on!@*/false || !!document.documentMode; // At least IE6
    if (isFirefox) return 1;
    if (isSafari) return 2;
    if (isChrome) return 3;
    if (isIE) return 4;
    if (isOpera) return 5;
    return 0;
  },
  lowercase1: function (text) {
    var value = text;
    if (value.length && value.charCodeAt(0) > 64 && value.charCodeAt(0) < 90)
      value = value.substr(0, 1).toLowerCase() + value.substr(1, value.length - 1);
    return value;
  },
  add_autosimplify: function (value) {
    var n = value.search(';');
    if (value.length == 0 || value[0] == '@')
      return value;
    if (n < 0 || n >= value.length) {
      var n = value.search('//');
      if (n < 0 || n >= value.length) {
        if (UI.python_mode)
          return 'add_autosimplify(@@' + value + ')';
        return 'add_autosimplify(' + value + ')';
      }
    }
    return value;
  },
  caseval: function (text) {
    if (!UI.ready) return ' Clic_on_Exec ';
    var docaseval = Module.cwrap('caseval', 'string', ['string']);
    var value = text;
    value = value.replace(/%22/g, '\"');
    value = UI.add_autosimplify(value);
    var s, err;
    //console.log('caseval',value);
    try {
      s = docaseval(value);
    } catch (err) {
      console.log(err);
    }
    // Module.print(text+ ' '+s);
    return s;
  },
  caseval_noautosimp: function (text) {
    if (!UI.ready) return ' Clic_on_Exec ';
    //console.log(text);
    var docaseval = Module.cwrap('caseval', 'string', ['string']);
    var value = text;
    value = value.replace(/%22/g, '\"');
    var s, err;
    try {
      s = docaseval(value);
    } catch (err) {
      console.log(err);
    }
    return s;
  },
  webworker: 0,
  withworker: 0,
  busy: 0,
  casevalcb: function (text, callback, args) {
    // prepare for webworker: casevalcb will run docaseval in a worker
    // 3d plotting does not work...
    if (UI.withworker && !!window.Worker) {
      if (!UI.webworker) {
        UI.webworker = new Worker("giacworker.js");
        console.log('worker created ');
        UI.webworker.onmessage = function (e) {
          var s = e.data[1];
          if (e.data[0] == 'cas') Module.print(s);
          if (e.data[0] == 'print') Module.print(s);
        }
      }
      // the worker will do the evaluation and post s
      UI.webworker.onmessage = function (e) {
        var s = e.data[1];
        UI.busy = 0;
        if (e.data[0] == 'cas') callback(s, args);
        if (e.data[0] == 'print') Module.print(s);
      }
      UI.busy = 1;
      UI.webworker.postMessage(['eval', text]);
      return;
      // STOP: myWorker.terminate()
    }
    var docaseval = Module.cwrap('caseval', 'string', ['string']);
    var value = UI.add_autosimplify(text);
    var s, err;
    //console.log(value);
    try {
      s = docaseval(value);
    } catch (err) {
    }
    // Module.print(text+ ' '+s);
    return callback(s, args);
  },
  history_cm: 0,
  ckenter: function (event, field) {
    //console.log(event.keyCode,event.shiftKey);
    var key = event.keyCode;
    if (key == 13 && event.shiftKey) {
      UI.insert(field, '\n');
      UI.indentline(field);
      return false;
    }
    if (key != 13 || event.shiftKey) return true;
    UI.reeval(field, '', true);
    return false;
  },
  ckenter_comment: function (event, field) {
    //console.log(event.keyCode,event.shiftKey);
    var key = event.keyCode;
    if (key == 13) UI.resizetextarea(field);
    var skipline = key != 13 || !event.ctrlKey;
    if (skipline && key == 13 && !event.shiftKey) {
      // check enter pressed at beginning of field
      var pos = field.selectionStart;
      var end = field.selectionEnd;
      if (pos == 0 && end == 0)
        skipline = false;
      // check enter pressed at end after a blank line
      var text = field.value;
      if (pos == text.length && end == pos) {
        if (pos >= 1 && text[pos - 1] == '\n')
          skipline = false;
      }
    }
    if (skipline) return true;
    UI.editcomment_end(field.nextSibling, true);
    var par = field.parentNode;
    par = par.parentNode;
    par = par.parentNode;
    par = par.parentNode.nextSibling;
    if (par == null) cmentree.focus();
    else {
      par = par.firstChild.firstChild.nextSibling.nextSibling;
      par = par.firstChild;
      if (par.style.display == 'none') { // commentaire
        par = par.nextSibling;
        par.click();
      }
      else
        par.focus();
    }
    return false;
  },
  restoresession: function (chaine, hist, asked, doexec) {
    if (!UI.ready) {
      window.setTimeout(UI.restoresession, 100, chaine, hist, asked, doexec);
      return;
    }
    UI.caseval_noautosimp("srand("+(new Date()).getTime()+")");
    //console.log(chaine,doexec);
    var clearcmd = true;
    var hashParams = chaine.split('&');
    if (hashParams.length == 0) return;
    for (var i = 0; i < hashParams.length; i++) {
      var s = hashParams[i];
      if (s.length) $id('startup_restore').style.display = 'none';
      if (s == 'exec') {
        console.log(s);
        doexec = true;
        continue;
      }
      s = s.replace(/___/g, '%');
      s = decodeURIComponent(s); //console.log(s);
      // s=s.replace('%0a','\n','g');
      s = s.replace(/%3b/g, ';');
      // s=s.replace('%3b',';','g');
      // s=s.replace('%3C','<','g');
      // s=s.replace('%3e','>','g');
      // s=s.replace('%5e','^','g');
      // s=s.replace('%20',' ','g');
      // s=s.replace('%27','\'','g');
      // s=s.replace('%22','\"','g');
      // s=s.replace('%C3%A9','é','g');
      // s=s.replace('%C3%A8','è','g');
      if (s.length && s.charAt(0) == '+') {
        s = s.substr(1);
        // if (s.substr(0,2)=='//' && s.substr(0,3)!='///') s='/'+s;
        // if (!asked) doexec=true;
        if (s.length) UI.eval_cmdline1(s, false);
        continue;
      }
      if (s.length && s.charAt(0) == '*') {
        if (!asked) doexec = true;
        var pos = s.search(',');
        var name = s.substr(1, pos - 1);
        // Module.print(name);
        s = s.substr(pos + 1, s.length - pos - 1);
        pos = s.search(',');
        var value = s.substr(0, pos);
        // Module.print(value);
        s = s.substr(pos + 1, s.length - pos - 1);
        pos = s.search(',');
        var mini = s.substr(0, pos);
        // Module.print(mini);
        s = s.substr(pos + 1, s.length - pos - 1);
        pos = s.search(',');
        var maxi = s.substr(0, pos);
        // Module.print(maxi);
        s = s.substr(pos + 1, s.length - pos - 1);
        UI.addcurseur(name, value, mini, maxi, s);
        continue;
      }
      var p = s.split('=');
      if (p[0] == '') continue;
      if (p[0] == 'lang') {
        p = p[1];
        if (p == 'fr')
          UI.langue = -1;
        if (p == 'en')
          UI.langue = -2;
        if (p == 'sp')
          UI.langue = -3;
        if (p == 'gr')
          UI.langue = -4;
        if (p == 'de')
          UI.langue = -5;
        var form = $id('config');
        if (UI.langue >= -5 && UI.langue <= -1) {
          form.lang[-UI.langue - 1].checked = true;
          if (UI.langue == -2) UI.langue = 0;
        }
        continue;
      }
      if (p[0] == 'from') {
        UI.mailto = decodeURIComponent(p[1]);
        continue;
      }
      if (p[0] == 'filename') {
        var out = $id("outputfilename");
        var s = decodeURIComponent(p[1]);
        out.value = s;
        //console.log(out);
        if (!UI.detectmob())
          out.style.width = (s.length * 10) + "px";
        continue;
      }
      if (p[0] == 'entree' || p[0] == 'cmentree') {
        cmentree.setValue(decodeURIComponent(p[1]));
        clearcmd = false;
        continue;
      }
      if (p[0] == 'codemirror') {
        if (p[1] == '0') {
          $id('config').usecm.checked = false;
          UI.set_config(false);
        }
        if (p[1] == '1') {
          $id('config').usecm.checked = true;
          UI.set_config(false);
        }
        continue;
      }
      var form = $id('config');
      if (p[0] == 'python') {
        if (p[1] == '0') {
          form.python_mode.checked = false;
	  form.python_xor.checked = false;
          UI.python_mode = 0;
        }
        if (p[1] == '1') {
          form.python_mode.checked = true;
	  form.python_xor.checked = false;
          UI.python_mode = 1;
        }
        if (p[1] == '2') {
          form.python_mode.checked = true;
	  form.python_xor.checked = true;
          UI.python_mode = 2;
        }
        UI.setoption_mode(cmentree);
        continue;
      }
      if (p[0] == 'radian') {
        if (p[1] == '0') {
          form.angle_mode.checked = false;
          window.setTimeout(UI.caseval_noautosimp, 100, 'angle_radian:=0');
          window.setTimeout(UI.set_settings, 300);
        }
        if (p[1] == '1') {
          form.angle_mode.checked = true;
        }
        continue;
      }
      if (p[0] == 'sqrt') {
        if (p[1] == '0') {
          form.sqrt_mode.checked = false;
        }
        if (p[1] == '1') {
          form.sqrt_mode.checked = true;
        }
        continue;
      }
      if (p[0] == 'digits') {
        var d = eval(p[1]);
        form.digits_mode.value = d;
        if (d != 12) {
          window.setTimeout(UI.caseval_noautosimp, 100, 'Digits:=' + d);
          window.setTimeout(UI.set_settings, 300);
        }
        continue;
      }
      $id(p[0]).value = decodeURIComponent(p[1]);
    } // end for (i=...)
    if (doexec) {
      UI.exec(hist, 0);
    }
    if (clearcmd && cmentree && cmentree.setValue) cmentree.setValue('');
    if (hist.firstChild != null) hist.firstChild.scrollIntoView();
    var cons = $id('restoresession');
    cons.style.display = 'none';
    //cmentree.focus();
  },
  link: function (start) {
    var s = UI.makelink(start);
    if ($id('variables').style.display != 'none') UI.listvars(3);
    //console.log(s);
    UI.createCookie('xcas_session', s, 365);
    if (s.length > 0) {
      var s2 = "#exec&" + s;
      var smail;
      if (UI.langue == -1)
        smail = UI.base_url + "xcasfr.html#exec&";
      else
        smail = UI.base_url + "xcasen.html#exec&";
      var filename = $id("outputfilename").value;
      var pos = filename.search('@');
      if (pos < 0 || pos >= filename.length)
        filename = UI.from + '@' + filename;
      if (pos == 0)
        filename = filename.substr(1, filename.length - 1);
      filename = 'filename=' + encodeURIComponent(filename) + '&';
      if (UI.from.length)
        filename += 'from=' + encodeURIComponent(UI.from) + '&';
      s = filename + s;
      //console.log(s);
      smail = smail + s;
      var sforum;
      if (UI.langue == -1) {
        sforum = UI.base_url + "xcasfr.html#exec&" + s;
        s = UI.base_url + "xcasfr.html#" + s;
      }
      else {
        sforum = UI.base_url + "xcasen.html#exec&" + s;
        s = UI.base_url + "xcasen.html#" + s;
      }
      //s=encodeURIComponent(s); // does not work innerHTML will add a prefix
      //var sforum=encodeURIComponent('[url]'+s+'[/url]');
      sforum = '[url=' + sforum + ']session Xcas[/url]';
      $id('theforumlink').innerHTML = sforum;
      var copy = "<button title=";
      copy += UI.langue == -1 ? "'Partager cette session sur le forum'" : "'Share this session on the forum'";
      copy += " class='bouton' onclick='var tmp=$id(\"theforumlink\"); tmp.style.display=\"inline\";tmp.select();document.execCommand(\"copy\");tmp.style.display=\"none\"; ";
      if (UI.forum_warn) {
        UI.forum_warn = false;
        copy += UI.langue == -1 ? "alert(\"Le lien de la session a été copié dans le presse-papier\");" : "alert(\"Clipboard contains a link to session\");";
      }
      copy += "var win=window.open(\"" + UI.forum_url + "\", \"_blank\");'>F</button>,";
      //console.log(copy);
      if (window.location.href.substr(0, 4) == 'file' && !UI.detectmob()) {
        $id('thelink').innerHTML = '<a title="Clone session" href="' + s + '" target="_blank">x2</a>, <a title="Local clone" href="' + s2 + '" target="_blank">local</a>,' + copy;//+',<a href="http://xcas.e.univ-grenoble-alpes.fr/XCAS/posting.php?mode=post&f=12&subject=session&message='+encodeURIComponent(sforum)+'" target="_blank">forum</a>,';
      }
      else
        $id('thelink').innerHTML = '<a href="' + s + '" target="_blank">x2</a>,' + (copy);
      var mailurl;
      if (UI.from.length > 9 && UI.from.substr(UI.from.length - 9, 9) == "gmail.com")
        mailurl = 'https://mail.google.com/mail/?view=cm&fs=1&tf=1&source=mailto&su=session+Xcas&to=' + UI.mailto;
      else
        mailurl = 'mailto:' + UI.mailto + '?subject=session Xcas';
      mailurl += '&body=Bonjour%0d%0aVeuillez suivre ce lien : <' + encodeURIComponent(smail) + '>';
      $id('themailto').innerHTML = '<a href="#" title="New session" target="_blank">&nbsp;+&nbsp;</a>,<a title="E-mail session" href="' + mailurl + '" target="_blank"> &#x2709; </a>,';
    }
  },
  rewritestring: function (s) {
    var res, i, l, ch;
    l = s.length;
    res = '';
    for (i = 0; i < l; ++i) {
      ch = s[i];
      if (ch == '&') {
        res += "%26";
        continue;
      }
      if (ch == '#') {
        res += "%23";
        continue;
      }
      if (i < l - 2 && ch == '%' && s[i + 1] != '7') {
        res += "___";
        continue;
      }
      res += s[i];
    }
    return res;
  },
  makelink: function (start) { // start=-1 Casio save
    console.log('makelink',start);
    var s = 'python=';
    if (UI.python_mode) s += (UI.python_mode+'&'); else s += '0&';
    var cur = $id('mathoutput').firstChild;
    var i = 0;
    var savepy=UI.python_mode;
    if (savepy)
      UI.caseval_noautosimp('python_compat(0)');
    var casiovars=UI.caseval_noautosimp('VARS(-1)');
    if (savepy)
      UI.caseval_noautosimp('python_compat('+savepy+')');
    casiovars += ';python_compat('+UI.python_mode+');angle_radian('+ ($id('config').angle_mode.checked?1:0)+');';
    var casioscript="",casioin=[];
    for (; cur; i++) {
      if (i >= start) {
        var field = cur.firstChild;
        field = field.firstChild;
        field = UI.skip_buttons(field);
        var fs = field.innerHTML;
        if (fs.length > 6 && fs.substr(0, 6) == "<span ") { // comment
          fs = field.firstChild.firstChild.value;
	  if (start==-1){
	    casioin.push('/*'+fs+'*/');
	    casioin.push('');
            cur = cur.nextSibling;
	    continue;
	  }
          fs = encodeURIComponent(fs);
          //fs=fs.replace(/\n/g,'%0a');
          //console.log(fs);
        }
        if (fs.length > 5) {
          var fs1 = fs.substr(0, 5);
          if (fs1 == "<form") {
            var pos1 = fs.search("<input");
            fs = fs.substr(pos1, fs.length - pos1);
            //console.log(fs);
            var pos1 = fs.search("value=");
            pos1 += 7;
            fs = fs.substr(pos1, fs.length - pos1);
            var pos2 = fs.search("\"");
            fs1 = fs.substr(0, pos2); // cursor name
	    if (start==-1) fs1='assume('+fs1;
            var pos1 = fs.search("value=");
            pos1 += 7;
            fs = fs.substr(pos1, fs.length - pos1);
            var pos2 = fs.search("\"");
            fs1 += start==-1?"=[":',';
	    fs1 += fs.substr(0, pos2); // current value
            var pos1 = fs.search("minname");
            pos1 += 7;
            fs = fs.substr(pos1, fs.length - pos1);
            var pos1 = fs.search("value=");
            pos1 += 7;
            fs = fs.substr(pos1, fs.length - pos1);
            var pos2 = fs.search("\"");
            fs1 += ',' + fs.substr(0, pos2); // min
            var pos1 = fs.search("maxname");
            pos1 += 7;
            fs = fs.substr(pos1, fs.length - pos1);
            var pos1 = fs.search("value=");
            pos1 += 7;
            fs = fs.substr(pos1, fs.length - pos1);
            var pos2 = fs.search("\"");
            fs1 += ',' + fs.substr(0, pos2); //max
            var pos1 = fs.search("value=");
            pos1 += 7;
            fs = fs.substr(pos1, fs.length - pos1);
            var pos2 = fs.search("\"");
            fs1 += ',' + fs.substr(0, pos2); // step
	    if (start==-1){
	      fs1 +='])';
              //console.log(fs1);
	      casioin.push(fs1);
	      casioin.push('');
	    }
            s += '*' + fs1 + '&';
            cur = cur.nextSibling;
            continue;
          }
        }
        var pos = fs.search("<textarea");
        if (pos >= 0 && pos < fs.length) {
          // var tmp=field.firstChild.value.replace(/\n/g,'%0a'); tmp=tmp.replace(';','%3b','g');
          // s += '+' + tmp.replace('&&',' and ','g') + '&';
          var tmp = field.firstChild.value;
	  if (start==-1){ // Casio export
	    if (tmp.indexOf('\n')!=-1 &&
		(tmp.indexOf('def')!=-1 || tmp.indexOf('nction')!=-1 || tmp.indexOf('{')!=-1))
	      casioscript += tmp+'\n';
	    else {
	      casioin.push(tmp);
	      if (field.nextSibling){
		field=field.nextSibling.firstChild;
		if (field){
		    field=field.nextSibling;
		  if (field){
		    fs = field.innerHTML;
		    casioin.push(fs);
		  }
		  else
		    casioin.push('Graphic object');
		}
	      }
	      else casioin.push("");
	    }
	  }
	  else tmp=encodeURIComponent(tmp);
          s += '+' + tmp + '&';
          cur = cur.nextSibling;
          continue;
        }
        pos = fs.search("UI.addhelp");
        if (pos >= 0 && pos < fs.length) {
          cur = cur.nextSibling;
          continue;
        }
	if (start==-1){
	  casioin.push('/*'+fs+'*/');
	  casioin.push('')
	}
        s += '+///' + fs + '&';
      }
      cur = cur.nextSibling;
    }
    // Module.print(s);
    s = s.replace(/\"/g, '%22');
    s = s.replace(/>/g, '%3e');
    if (start==-1) return [casiovars,casioscript,casioin];
    return s;
  },
  canvas_mousemove: function (event, no) {
    if (UI.canvas_pushed) {
      // Module.print(event.clientX);
      if (UI.canvas_lastx != event.clientX) {
        if (event.clientX > UI.canvas_lastx)
          UI.giac_renderer('r' + no);
        else
          UI.giac_renderer('l' + no);
        UI.canvas_lastx = event.clientX;
      }
      if (UI.canvas_lasty != event.clientY) {
        if (event.clientY > UI.canvas_lasty)
          UI.giac_renderer('d' + no);
        else
          UI.giac_renderer('u' + no);
        UI.canvas_lasty = event.clientY;
      }
    }
  },
  show_menu: function () {
    if ($id('keyboardfunc').style.display == 'inline') {
      $id('keyboardfunc').style.display = 'none';
    }
    else {
      $id('keyboardfunc').style.display = 'inline';
      $id('keyboardfunc').scrollIntoView();
      $id('keyboard').style.display = 'none';
      $id('alpha_keyboard').style.display = 'none';
      $id('progbuttons').style.display = 'none';
      var tab = ['boucle', 'seq', 'rand', 'series', 'int', 'sum', 'limit', 'plotfunc', 'plotparam',
        'plotpolar', 'plotimplicit', 'plotfield', 'tabvar', 'test', 'prog', 'solve',
        'rsolve', 'diff', 'matr', 'suites', 'arit', 'geo', 'linalg', 'rewrite',
        'graph', 'calculus'];
      var s = tab.length, k;
      for (k = 0; k < s; ++k) {
        var tmp = $id('assistant_' + tab[k]);
        if (tmp.style.display == 'block') {
          tmp.style.display = 'none';
          UI.focused = UI.savefocused;
        }
      }
    }
    if (UI.focusaftereval) {
      UI.focused.focus();
    }
  },
  show_config: function () {
    var form = $id('config');
    form.style.display = 'inline';
  },
  editline: false,
  set_editline: function (field, b) {
    UI.editline = b;
    if (field) {
      if (field.parentNode)
        field = field.parentNode;
      else field = field.getTextArea().parentNode;
    }
    if (field) {
      //UI.switch_buttons(field,true);
      field = field.firstChild;
      field = field.nextSibling;
      if (field.CodeMirror)
        field = field.nextSibling;
      //console.log(b,field.id);
      if (field && field.id == "")
        field.style.display = b ? 'inline' : 'none';
    }
  },
  set_settings: function () {
    var form = $id('config');
    var hw = window.innerWidth;
    //console.log(hw);
    if (hw >= 700) {
      var cfg = $id('curcfg');
      var s = "";
      s += UI.python_mode ? (UI.python_mode==2?' pyth xor ':'pyth ** ' ): 'xcas ';
      s += form.angle_mode.checked ? 'rad ' : 'deg ';
      s += form.digits_mode.value;
      if (form.complex_mode.checked) s += " ℂ"; // else s+=" ℝ";
      if (form.sqrt_mode.checked) s += " &radic;";
      cfg.innerHTML = s;
    }
  },
  set_config_width: function () {
    UI.set_settings();
    var form = $id('config');
    var hw = window.innerWidth, hh = window.innerHeight;
    if (hw >= 1000) {
      hw = hw - 50;
      UI.focusaftereval = true;
    }
    if (hw <= 500) {
      UI.focusaftereval = false;
      $id('exportbutton').style.display = 'none';
    }
    form.outdiv_width.value = Math.floor(hw / 2);
    $id('mathoutput').style.maxWidth = hw + 'px';
    var mh = Math.floor(hh * .55);
    if ($id('consolediv').style.display != 'block') mh = Math.floor(hh * .655);
    var mob = UI.detectmob();
    if (UI.editline && mob) mh = Math.floor(hh * .8);
    //console.log('set_config_width hh',$id('divoutput').style.maxHeight,mh);
    $id('divoutput').style.maxHeight = mh + 'px';
    var w = form.outdiv_width.value, h;
    // Module.print(hw);Module.print(w);
    if (w > hw - 300) w = hw - 300;
    var hi = hw - w - 153;
    if (!UI.qa) {
      hi = hw - 130;
      w = hi;
    }
    var hb = 29;
    if (hh >= 400) hb = 32;
    if (hh >= 500) hb = 34;
    if (hh > 600) hb = 37;
    //mh=form.outdiv_height.value;
    var cms = mh - 2 * hb; // codemirror scrollbar max height
    if (UI.focused == cmentree) cms = Math.floor(hw * .67);
    s = 'h1,h2,h3 { display:inline; font-size:1em;}\ninput[type="number"] { width:40px;}\n .outdiv { width:' + w + 'px; max-height: ' + mh + 'px;  overflow: auto;}\n.filenamecss {width:80px;height:20px}\n.historyinput {width:' + hi + 'px;}\n.bouton{vertical-align:center; height:' + (mob ? hb : 29) + 'px;}\n.CodeMirror-scroll {height:auto; max-height:' + cms + 'px;}\n.CodeMirror {border: 1px solid black;  height:auto; min-width:' + hi + 'px;}\n  dt {font-family: monospace; color: #666;}';
    //console.log(mh,cms);// Module.print(s);
    //Module.print(mh,cms);
    var st = $id('document_style');
    st.innerHTML = s;
    var kbd_l = ["add_newline", "add_infer", "add_super", "add_left_par", "add_right_par",
      "add_i", "add_7", "add_8", "add_9", "add_/",
      "add_semi", "add_abc", "add_pi",
      "add_4", "add_5", "add_6", "add_*", "add_beg", "add_end", "add_:",
      "add_,", "add_xtn", "add_1", "add_2",
      "add_3", "add_-", "copy_button", "curseur_up", "add-=",
      "add_dosel", "add_ln", "add_e", "add_0", "add_.",
      "add_+", "curseur_down", "shift_key", "add_sin", "add_cos",
      "add_tan", "add_sqrt", "add_^"];
    var kbd_a = ["add_alpha_a", "add_alpha_b", "add_alpha_c", "add_alpha_d", "add_alpha_e",
      "add_alpha_f", "add_alpha_j", "add_alpha_n", "add_alpha_r", "add_alpha_u", "add_alpha_x",
      "add_alpha_g", "add_alpha_k", "add_alpha_o", "add_alpha_s", "add_alpha_v", "add_alpha_y",
      "add_alpha_h", "add_alpha_l", "add_alpha_p", "add_alpha_t", "add_alpha_w", "add_alpha_z",
      "add_alpha_i", "add_alpha_m", "add_alpha_q", "add_alpha_{", "add_alpha_}",
      "add_alpha_space", "add_alpha_"];
    var kbd_math = ["add_arit", "add_matr", "add_graph", "add_calculus",
      "add_linalg", "add_stats", "add_tableur", "add_rewritetrig", "add_solve",
      "add_mathcomment", "add_rand", "add_geo", "add_rsolve",
      "add_seq", "add_tabvar",
      "add_curseur"];
    var kbd_prog = ["add_//", "add_nlprog", "add_listechaine", "add_tortue", "add_test",
      "add_boucle", "add_function", "add_debug", "add_efface", "add_avance",
      "add_recule", "add_tourne_gauche", "add_tourne_droite",
      "add_pas_de_cote", "add_saute", "add_crayon", "add_rond", "add_disque",
      "add_rectangle_plein", "add_triangle_plein", "add_ecris", "add_repete", "tortue_maillage", "tortue_clear"];
    w = Math.floor(hw / 12) - 1;
    w = w + "px";
    h = Math.floor(hh / 20);
    if (h < 34) h = 34;
    h = h + "px"; // console.log(hw,w,hh,h);
    for (var i = 0; i < kbd_l.length; i++) {
      $id(kbd_l[i]).style.width = w;
      $id(kbd_l[i]).style.height = h;
    }
    for (var i = 0; i < kbd_a.length; i++) {
      $id(kbd_a[i]).style.width = w;
      $id(kbd_a[i]).style.height = h;
    }
    w = Math.floor(hw / 9) - 1;
    if (w < 30) w = 30;
    w = w + "px";
    h = Math.floor(hh / 20);
    if (h < 35) h = 35;
    h = h + "px"; // console.log(hw,w,hh,h);
    for (var i = 0; i < kbd_prog.length; i++) {
      $id(kbd_prog[i]).style.width = w;
      $id(kbd_prog[i]).style.height = h;
    }
    w = Math.floor(hw / 9) - 3;
    if (w < 30) w = 30;
    w = w + "px";
    h = Math.floor(hh / 20);
    if (h < 35) h = 35;
    h = h + "px"; // console.log(hw,w,hh,h);
    for (var i = 0; i < kbd_math.length; i++) {
      $id(kbd_math[i]).style.width = w;
      $id(kbd_math[i]).style.height = h;
    }
    w = Math.floor(hw / 9) - 3;
    if (w < 30) w = 30;
    w = w + "px";
  },
  config_string: function () {
    var form = $id('config');
    UI.from = form.from.value;
    UI.mailto = form.to.value;
    //console.log(UI.from);
    if (form.qa.checked) UI.qa = true; else UI.qa = false;
    if (form.usecm.checked) UI.usecm = true; else UI.usecm = false;
    UI.switchcm();
    if (form.fixeddel.checked) UI.fixeddel = true; else UI.fixeddel = false;
    UI.set_config_width();
    var s;
    var st = $id('document_style');
    if (form.online_doc.checked)
      UI.docprefix = UI.base_url + 'giac/doc/' + (UI.langue == -1 ? 'fr/cascmd_fr/' : 'en/cascmd_en/');
    else
      UI.docprefix = "file://" + form.doc_path.value;
    if (form.prettyprint.checked) UI.prettyprint = true; else UI.prettyprint = false;
    if (form.worker_mode.checked) {
      if (!UI.withworker) {
        if (UI.langue == -1)
          alert('Session redemarree (variables remises a 0). Les calculs seront faits par un worker.');
        else
          alert('Session restarted (variables purged). Computation are performed with a worker now.');
      }
      UI.withworker = true;
    } else {
      if (UI.withworker) {
        if (UI.langue == -1)
          alert('Session redemarree (variables remises a 0).');
        else
          alert('Session restarted (variables purged).');
      }
      UI.withworker = false;
    }
    if (UI.withworker) $id('stop_button').style.display = 'inline'; else $id('stop_button').style.display = 'none';
    UI.caseval_noautosimp("autosimplify(" + form.autosimp_level.value + ")");
    //Module.print(st.innerHTML);
    s = 'Digits:=';
    s += form.digits_mode.value;
    s += '; angle_radian:=';
    if (form.angle_mode.checked) s += 1; else s += 0;
    s += '; complex_mode:=';
    if (form.complex_mode.checked) s += 1; else s += 0;
    s += '; with_sqrt(';
    if (form.sqrt_mode.checked) s += 1; else s += 0;
    s += '); step_infolevel(';
    if (form.step_mode.checked) s += 1; else s += 0;
    s += ');python_compat(';
    if (form.python_mode.checked){ if (form.python_xor.checked) s+=2; else s += 1; } else s += 0;
    s += ');';
    // Module.print(s);
    return s;
  },
  set_config: function (setcm_mode) { // b==true if we set cmentree
    var form = $id('config');
    UI.canvas_w = form.canvas_w.value;
    UI.canvas_h = form.canvas_h.value;
    var s = UI.config_string();
    //console.log(form.wasm_mode);
    UI.addhelp(' ', s);
    form.style.display = 'none';
    if (UI.focusaftereval) UI.focused.focus();
    var test;
    for (test = 0; test < 5; ++test) {
      if (form.lang[test].checked) break;
    }
    test++;
    if (test > 5 || test == 2) test = 0;
    UI.langue = -test;
    UI.createCookie('xcas_lang', test, 10000);
    UI.createCookie('xcas_from', form.from.value, 10000);
    UI.createCookie('xcas_to', form.to.value, 10000);
    UI.createCookie('xcas_digits', form.digits_mode.value, 10000);
    UI.createCookie('xcas_angle_radian', form.angle_mode.checked ? 1 : -1, 10000);
    UI.warnpy = form.warnpy_mode.checked;
    UI.createCookie('xcas_warnpy', form.warnpy_mode.checked ? 1 : -1, 10000);
    UI.python_mode = form.python_mode.checked?(form.python_xor.checked?2:1):0;
    UI.createCookie('xcas_python_mode', UI.python_mode, 10000);
    UI.set_settings();
    if (setcm_mode) {
      UI.setoption_mode(cmentree);
    }
    $id('add_//').value = UI.python_mode ? '#' : '//';
    if (!UI.kbdshift) $id('add_:').value = UI.python_mode ? ':' : ':=';
    if (cmentree.type != 'textarea') cmentree.options.indentUnit = UI.python_mode ? UI.python_indent : 2;
    UI.createCookie('xcas_complex_mode', form.complex_mode.checked ? 1 : -1, 10000);
    UI.createCookie('xcas_with_sqrt', form.sqrt_mode.checked ? 1 : -1, 10000);
    UI.createCookie('xcas_step_infolevel', form.step_mode.checked ? 1 : -1, 10000);
    UI.createCookie('xcas_autosimplify', form.autosimp_level.value, 10000);
    UI.createCookie('xcas_docprefix', UI.docprefix, 10000);
    UI.createCookie('xcas_withworker', UI.withworker ? 1 : -1, 10000);
    UI.createCookie('xcas_wasm', form.wasm_mode.checked ? 1 : -1, 10000);
    UI.createCookie('xcas_prettyprint', UI.prettyprint ? 1 : -1, 10000);
    UI.createCookie('xcas_qa', UI.qa ? 1 : -1, 10000);
    UI.createCookie('xcas_usecm', UI.usecm ? 1 : -1, 10000);
    UI.createCookie('xcas_fixeddel', UI.fixeddel ? 1 : -1, 10000);
    UI.createCookie('xcas_canvas_w', form.canvas_w.value, 10000);
    UI.createCookie('xcas_canvas_h', form.canvas_h.value, 10000);
    //UI.createCookie('xcas_outdiv_width',form.outdiv_width.value,10000);
    //UI.createCookie('xcas_outdiv_height',form.outdiv_height.value,10000);
    UI.createCookie('xcas_matrix_maxrows', form.matr_cfg_rows.value, 10000);
    UI.createCookie('xcas_matrix_maxcols', form.matr_cfg_cols.value, 10000);
    UI.createCookie('xcas_matrix_textarea', form.matr_textarea.checked ? 1 : -1, 10000);
    UI.assistant_matr_setmatrix(form.matr_cfg_rows.value, form.matr_cfg_cols.value);
    UI.assistant_matr_setdisplay();
    //$id('settings').style.backgroundImage = "url('config.png')";
    //document.body.style.backgroundImage = "url('logo.png')";
  },
  savesession: function (i) {
    // console.log('save_session',i);
    var s;
    filename = $id("outputfilename").value;
    document.title = "Xcas_" + filename;
    if (i == 2) {
      UI.createCookie('xcas__' + filename, UI.makelink(0), 9999);
      console.log(UI.listCookies());
      return;
    }
    if (i == 1) {
      s=UI.makelink(-1);
      if (s[1].length==0)
	s[1]='\n';
      var l=4+s[0].length+4+s[1].length;
      for (var j=0;j<s[2].length;j++)
	l+=6+s[2][j].length;
      console.log(l);
      var buf=new Uint8Array(l+2);
      buf[0]=0;
      l=s[0].length;
      buf[1]=l/65536;
      buf[2]=(l/256)%256;
      buf[3]=l%256;
      for (var j=0;j<s[0].length;++j){
	buf[4+j]=s[0].charCodeAt(j);
      }
      var pos=4+l;
      buf[pos]=0;
      l=s[1].length;
      buf[pos+1]==l/65536;
      buf[pos+2]=(l/256)%256;
      buf[pos+3]=l%256;
      for (var j=0;j<s[1].length;++j){
	buf[pos+4+j]=s[1].charCodeAt(j);
      }
      pos +=4+l;
      for (var i=0;i<s[2].length;++i){
	var S=s[2][i];
	l=S.length;
	if (l==0) continue;
	buf[pos]=l/256;
	buf[pos+1]=l%256;
	buf[pos+4]=(i%2)?1:0;
	buf[pos+5]=1;
	for (var j=0;j<S.length;++j){
	  buf[pos+6+j]=S.charCodeAt(j);
	}
	pos += 6+l;
      }
      var blob = new Blob([buf]);
      filename += ".xw";
      saveAs(blob, filename);
      return;
      // Casio change: make a Int8Array
      // create Blob([Int8Array_varname]) or Blob([Int8Array_varname],{type: "application/octet-stream"}))
      s = $id("fulldocument").innerHTML;
      s = '<html id="fulldocument" manifest="xcas.appcache">' + s + '</html>';
    }
    else {
      s = $id("mathoutput").innerHTML;
    }
    var blob = new Blob([s], {type: "text/plain;charset=utf-8"});
    if (i == 1) filename += ".html"; else filename += ".xw";
    saveAs(blob, filename);
  },
  show_history123: function () {
    $id('history1').style.display = 'inline';
    //$id('history2').style.display='block';
    $id('history4').style.display = 'block';
    $id('startup').style.display = 'none';
    $id('startup1').style.display = 'none';
    $id('startup2').style.display = 'none';
  },
  hide_show: function (tmp) {
    if (tmp.style.display == 'none')
      tmp.style.display = 'block';
    else
      tmp.style.display = 'none'
  },
  remove_extension: function(name){
    var s=name.length,i;
    for (i=s-1;i>=0;--i){
      if (name[i]=='.')
	break;
    }
    if (i>0)
      return name.substr(0,i);
    return name;
  },
  unsignedchar:function(s,pos){
    var r=s.charCodeAt(pos);
    var r1=(r+256)%256;
    return r1;
  },
  loadfile: function (oFiles) {
    var nFiles = oFiles.length;
    for (var nFileId = 0; nFileId < nFiles; nFileId++) {
      console.log('load',oFiles[nFileId].name);
      $id('outputfilename').innerHTML=UI.remove_extension(oFiles[nFileId].name);
      var reader = new FileReader();
      reader.readAsBinaryString(oFiles[nFileId]);//reader.readAsText(oFiles[nFileId]);
      var s;
      reader.onloadend = function (e) {
        s = e.target.result;
        if (s.length > 7 && s.substr(0, 7) == '<tbody>') {
          UI.show_history123();
          $id("mathoutput").innerHTML += s;
          if (confirm(UI.langue == -1 ? 'Ex&eacute;cuter les commandes de l\'historique?' : 'Execute history commands?'))
            UI.exec($id('mathoutput'), 0);
          // Module.print(s);
        }
        else {
	  // Casio change ?reader.readAsArrayBuffer or readAsBinaryString
	  if (s.charCodeAt(0)==0){
	    var editpos=0;
	    var editl=UI.unsignedchar(s,1);
	    editl=editl*256+UI.unsignedchar(s,2);
	    editl=editl*256+UI.unsignedchar(s,3);
	    var edits=s.substr(4,editl);
	    //console.log(edits);
	    var py=edits[editl-19];
	    var rad=edits[editl-3];
	    //console.log(edits,py,rad);
	    edits=edits.substr(0,edits.length-34);
	    UI.python_mode=0;
	    if (edits.length)
	      UI.caseval(edits);
	    UI.python_mode=py;
	    editpos=4+editl;
	    editl=UI.unsignedchar(s,editpos);
	    // console.log(0,editl);
	    for (var i=1;i<=3;i++){
	      editl=editl*256+UI.unsignedchar(s,editpos+i);
	      // console.log(i,editl);
	    }
	    edits=s.substr(editpos+4,editl);
	    // console.log('script ',editl,edits,edits.length);
	    if (edits=='\n')
	      edits=UI.python_mode?'def\n':'function\nffunction';
	    if (edits.length)
	      UI.eval_cmdline1(edits,true);
	    editpos += 4+editl;
	    while (editpos<s.length-4){
	      editl=UI.unsignedchar(s,editpos);
	      editl=editl*256+UI.unsignedchar(s,editpos+1);
	      if (editl==0) break;
	      var t=UI.unsignedchar(s,editpos+4);
	      var ro=UI.unsignedchar(s,editpos+5);
	      edits=s.substr(editpos+6,editl);
	      //console.log(t,ro,edits);
	      if (edits.length>=2){
		if (edits[0]=='#')
		  edits='///'+edits.substr(1,edits.length-1);
		else {
		  if (edits[0]=='/'){
		    if (edits[1]=='/')
		      edits ='/'+edits;
		    else {
		      if (edits[1]=='*' && edits.length>=4){
			edits='///'+edits.substr(2,edits.length-4);
		      }
		    }
		  }
		}
	      }
	      if (edits.length && t==0){
		var done=false;
		// detect assume(a=[cur,min,max,step])
		if (edits.length>=15 && edits.substr(0,7)=="assume("){
		  var pos=edits.search("=");
		  var name,value,mini,maxi,step;
		  if (pos>=8 && pos<edits.length){
		    name=edits.substr(7,pos-7);
		    //console.log(name);
		    // skip =[
		    edits=edits.substr(pos+2,edits.length-pos-2);
		    pos=edits.search(',');
		    if (pos>=1 && pos<edits.length){
		      value=edits.substr(0,pos);
		      //console.log(value);
		      edits=edits.substr(pos+1,edits.length-pos-1);
		      pos=edits.search(',');
		      if (pos>=1 && pos<edits.length){
			mini=edits.substr(0,pos);
			//console.log(mini);
			edits=edits.substr(pos+1,edits.length-pos-1);
			pos=edits.search(',');
			if (pos>=1 && pos<edits.length){
			  maxi=edits.substr(0,pos);
			  //console.log(maxi);
			  edits=edits.substr(pos+1,edits.length-pos-1);
			  pos=edits.search(']');
			  if (pos>=1 && pos<edits.length){
			    step=edits.substr(0,pos);
			    //console.log(name,value,mini,maxi,step);
			    UI.addcurseur(name, value, mini, maxi, step);
			    done=true;
			  }
			}
		      }
		    }
		  }
		}
		if (!done)
		  UI.eval_cmdline1(edits,true);
	      }
	      editpos+= 6+editl;
	    }
	  }
	  else
	    alert(UI.langue == -1 ? 'Format de document invalide' : 'Invalid document format');
	}
        if (UI.focusaftereval) UI.focused.focus();
      }
    }
  },
  show_level_answers: function (level, b) {
    var cur = level.firstChild;
    cur = cur.firstChild;
    cur = UI.skip_buttons(cur); // skip buttons
    var s = cur.innerHTML;
    var pos = s.search("<textarea");
    if (pos < 0 || pos >= s.length) return;
    cur = cur.nextSibling; // skip entry field
    if (b)
      cur.style.display = 'inherit';
    else
      cur.style.display = 'none';
  },
  show_answers: function (b) {
    var out = $id('mathoutput');
    var cur = out.firstChild;
    while (cur) {
      UI.show_level_answers(cur, b);
      cur = cur.nextSibling;
    }
    if (UI.focusaftereval) UI.focused.focus();
  },
  is_alphan: function (c) {
    return (c >= 48 && c <= 57) || (c >= 65 && c <= 91) || (c >= 97 && c <= 123) || c == 95;
  },
  erase_button: function (newline) {
    var s = '<td><button class="bouton" onclick=\'UI.erase(this)\' title="';
    s += UI.langue == -1 ? 'placer ce niveau dans la corbeille' : 'throw this level in the trash';
    s += '">&#x232b;</button>';
    /* if (newline)
       s+='<br>';
       s +='<button class="bouton" onclick=\'UI.reeval(UI.focused,"",false)\'>ok</button>';
    */
    s += '</td></tr>';
    return s;
  },
  move_buttons: function (newline) {
    var s = '<tr onmouseenter="UI.switch_buttons(this,true)" onmouseleave="if (!UI.fixeddel) UI.switch_buttons(this,false)">';
    // s += '<td>&bull;</td>'; // f=f.nextSibling in switch_buttons
    if (newline) {
      s += '<td><button style="width:20px;height:28px;" onclick="UI.moveup(this)" title="';
      s += UI.langue == -1 ? 'd&eacute;placer vers le haut' : 'move level up';
      s += '">↑</button><br><button style="width:20px;height:28px;" onclick="UI.movedown(this)" title="';
      s += UI.langue == -1 ? 'd&eacute;placer vers la bas' : 'move level down';
      s += '">↓</button></td><td></td>';
    }
    else {
      s += '<td><button style="width:20px;height:16px;" onclick="UI.moveup(this)" title="';
      s += UI.langue == -1 ? 'd&eacute;placer vers le haut' : 'move level up';
      s += '">↑</button><button style="width:20px;height:16px;" onclick="UI.movedown(this)" title="';
      s += UI.langue == -1 ? 'd&eacute;placer vers la bas' : 'move level down';
      s += '">↓</button></td><td><button style="width:20px;height:16px;" onclick="for (var i=0;i<5;i++) UI.moveup(this)" title="';
      s += UI.langue == -1 ? 'd&eacute;placer de 5 vers le haut' : 'move level 5 times up';
      s += '">↑↑</button><button style="width:20px;height:16px;" onclick="for (var i=0;i<5;i++) UI.movedown(this)" title="';
      s += UI.langue == -1 ? 'd&eacute;placer de 5 vers la bas' : 'move level 5 times down';
      s += '">↓↓</button></td>';
    }
    return s;
  },
  skip_buttons: function (field) {
    return field.nextSibling.nextSibling;
  },
  addplotfunc: function () {
  },
  paramname: 97,
  curseurhtml: function (name, mini, maxi, step, value) {
    var s = UI.move_buttons(!UI.qa);
    s += '<td colspan=3>';
    s += '<form onsubmit="setTimeout(function() { rangename.value=valname.value; rangename.step=stepname.value; valname.step=stepname.value;rangename.min=minname.value; rangename.max=maxname.value;UI.eval_below(name.form,name.value,rangename.value);}); return false;">';
    s += '<input class="curseur" type="text" name="name" size="1" value=\'' + name + '\'>';
    s += '=' + '<input class="curseur" type="number" name="valname" onchange="valname.innerHTML=valname.value" value=\'' + value + '\' step=\'' + step + '\'>';
    s += '<input class="curseur" type="submit" value="ok">'
    s += '&nbsp;<input class="curseur" type="button" value="--&nbsp;" onclick="rangename.value=valname.value -= 10*stepname.value;UI.eval_below(form,form.name.value,valname.value);">';
    s += '<input class="curseur" type="button" value="&nbsp;-&nbsp;&nbsp;" onclick="rangename.value=valname.value -= stepname.value;UI.eval_below(form,form.name.value,valname.value);">';
    s += '&nbsp;<input class="curseur" type="button" value="&nbsp;&nbsp;+&nbsp;" onclick="rangename.value=valname.value -= -stepname.value;UI.eval_below(form,form.name.value,valname.value);">';
    s += '<input class="curseur" type="button" value="&nbsp;++" onclick="rangename.value=valname.value -= -10*stepname.value;UI.eval_below(form,form.name.value,valname.value);">';
    s += '&nbsp;<input class="curseur" type="button" value="&#x270e;" onclick="var tmp=nextSibling; if (tmp.style.display==\'none\') tmp.style.display=\'inline\'; else tmp.style.display=\'none\';">';
    s += '<span style="display:none">&nbsp;<input class="curseur" type="number" name="minname" value=\'' + mini + '\' step=\'' + step + '\'>';
    if (window.innerWidth < 600)
      s += '<input class="curseur" type="range" style="display:none" name="rangename" onclick="valname.value=value;UI.eval_below(form,form.name.value,value);" value=' + value + ' min=' + mini + ' max=' + maxi + ' step=' + step + '>';
    else
      s += '<input class="curseur" type="range" name="rangename" onclick="valname.value=value;UI.eval_below(form,form.name.value,value);" value=' + value + ' min=' + mini + ' max=' + maxi + ' step=' + step + '>';
    s += '<input class="curseur" type="number" name="maxname" value=\'' + maxi + '\' step=\'' + step + '\'>&nbsp;';
    s += '(pas <input class="curseur" type="number" name="stepname" value=\'' + step + '\' step=\'' + step / 100 + '\'>)</span>';
    s += '</form>';
    s += '</td>';
    s += UI.erase_button(!UI.qa);
    return s;
  },
  addcurseur: function (name, value, mini, maxi, step) {
    UI.show_history123();
    UI.caseval_noautosimp('assume(' + name + '=' + value + ')');
    var s = UI.curseurhtml(name, mini, maxi, step, value);
    var out = $id('mathoutput');
    //Module.print(s);
    out.innerHTML += s;
    UI.link(0);
    UI.scrollatend(out.parentNode);
    if (UI.focusaftereval) UI.focused.focus();
  },
  exec_history: function () {
    alert(UI.langue == -1 ? 'Historique!' : 'History!');
  },
  svg_counter: 0,
  savesvg: function (field) {
    var s = field.innerHTML;
    s = '<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>\n' + s;
    var blob = new Blob([s], {type: "text/plain;charset=utf-8"});
    filename = $id("outputfilename").value;
    ++UI.svg_counter;
    filename += UI.svg_counter + ".svg";
    saveAs(blob, filename);
  },
  zoom: function (field, scale) {
    var prev = field.parentNode.previousSibling.lastChild;
    var ps = prev.innerHTML;
    if (ps.length > 22 && ps.substr(0, 17) == ' <canvas id=\"gl3d') {
      ps = ps.substr(18, 7);
      var pos = ps.search('"');
      if (pos > 0 && pos < 7) {
        ps = ps.substr(0, pos);
        if (scale > 1) UI.giac_renderer('-' + ps); else UI.giac_renderer('+' + ps);
        return;
      }
    }
    if (prev.firstChild) prev = prev.firstChild;
    // firstChild not required with all browsers
    var box = prev.viewBox.baseVal;
    var w = box.width / 1.2, h = box.height / 1.2;
    var x = box.x + w / 10, y = box.y + h / 10;
    //Module.print('current w/h '+w+','+h);
    var cx = x + w / 2, cy = y + h / 2; // center
    w = scale * w;
    h = scale * h; // new scales
    // Module.print('new center'+cx+','+cy);
    // Module.print('new w/h'+w+','+h);
    x = cx - w / 2;
    y = cy - h / 2;
    cx = x + w;
    cy = y + h;
    // reeval commandline with gl_x=x..cx and gl_y=y..cy
    var postcmd = ';gl_x=' + x + '..' + cx + ';gl_y=' + y + '..' + cy + ';';
    //Module.print(prev.parentNode.parentNode.previousSibling.innerHTML);
    //Module.print(postcmd);
    if (prev.parentNode.previousSibling) {
      //Module.print(prev.parentNode.parentNode.firstChild.innerHTML);
      UI.reeval(prev.parentNode.parentNode.firstChild, postcmd, false);
    }
    else
      UI.reeval(prev.parentNode.parentNode.previousSibling.firstChild, postcmd, false);
  },
  eval_cmdline: function () {
    UI.set_config_width();
    var value;
    $id('keyboardfunc').style.display = 'none';
    $id('keyboard').style.display = 'none';
    $id('alpha_keyboard').style.display = 'none';
    if (cmentree.type != 'textarea') value = cmentree.getValue(); else value = entree.value;
    UI.eval_cmdline1(value, true);
  },
  eval_cmdline1: function (value, docaseval) {
    if (!UI.ready) {
      alert(UI.langue == -1 ? 'Veuillez patienter pendant la préparation' : 'Please wait until system is ready');
      return;
    }
    UI.set_locale();
    // value=UI.lowercase1(value);
    var out;
    // suppress leading non ascii char
    var n = 0;
    for (; n < value.length; n++) {
      if (value.charCodeAt(n) > 32) break;
    }
    value = value.substr(n, value.length - n);
    for (n = value.length - 1; n >= 0; n--) {
      if (value.charCodeAt(n) > 32 || value.charCodeAt(n) == 10) break;
    }
    value = value.substr(0, n + 1);
    if (value.charCodeAt(n) != 10) {
      if (cmentree.type != 'textarea')
        cmentree.setValue(value);
      else
        entree.value = value;
    }
    var s = ' ';
    if (value.length >= 3 && value.substr(0, 3) == '///') {
      out = value;
    } else {
      if (docaseval) {
        if (UI.busy) {
          out = UI.langue == -1 ?' Le moteur de calcul est occupe.':' Kernel is busy.';
          s = out;
        }
        else {
          out = UI.casevalcb(value, UI.eval_cmdline1cb, value);
          return;
        }
      }
      else {
        out = UI.langue == -1 ?' Non evalue. Cliquer sur Exec pour evaluer. ':' Not evaled. Click Exec to eval. ';
        s = out;
      }
    }
    UI.eval_cmdline1end(value, out, s);
  },
  eval_cmdline1end: function (value, out, s) {
    var add = UI.addinput(value, out, s);
    //var s=UI.caseval_noautosimp('mathml(quote('+value+'),1)');
    //add += '&nbsp;&nbsp;'+s.substr(1,s.length-2);
    //Module.print(value+'  ->  '+out);
    if (UI.focusaftereval) cmentree.focus();
    if (cmentree.type != 'textarea') cmentree.execCommand('selectAll');
    else {
      var f = document.activeElement;
      cmentree.select();
      f.focus();
      UI.selection = cmentree.value;
    }
    //$id('canvas').focus();
    var mathoutput = $id('mathoutput');
    var tr = document.createElement("TABLE");
    tr.innerHTML += add;
    mathoutput.appendChild(tr.firstChild);
    if (value.length == 0 || (value.length >= 3 && value.substr(0, 3) == '///')) {
      var f = mathoutput.lastChild.firstChild.firstChild.nextSibling.nextSibling.firstChild.nextSibling;
      //console.log(f);
      f.click();
      //UI.selectionne();
    }
    // mathoutput.innerHTML += add;
    UI.render_canvas(mathoutput.lastChild);
    UI.scrollatend(mathoutput.parentNode);
    UI.link(0);
    if (UI.prettyprint && UI.usemathjax && UI.histcount > 0)
    //console.log('"hist'+(UI.histcount-1)+'"');
      MathJax.Hub.Queue(["Typeset", MathJax.Hub, '"hist' + (UI.histcount - 1) + '"']);
  },
  eval_cmdline1cb: function (out, value) {
    var s;
    //console.log(out);
    if (out.length > 5 && (out.substr(1, 4) == '<svg' || out.substr(0, 5) == 'gl3d ' || out.substr(0, 5) == 'gr2d(')) {
      //console.log(s);
      s = out;
      out = 'Done_graphic';
    }
    else {
      if (out.length > 1 && out[out.length - 1] == ';')
        out = out.substr(0, out.length - 1);
      if (out[0] == '"')
        s = 'text ' + out;
      else {
        if (UI.prettyprint) {
          if (UI.usemathjax)
            s = 'latex(quote(' + out + '))';
          else
            s = 'mathml(quote(' + out + '),1)'; //Module.print(s);
          //console.log(out,s);
          if (out.length > 10 && out.substr(0, 10) == 'GIAC_ERROR')
            s = '"' + out.substr(11, out.length - 11) + '"';
          else s = UI.caseval_noautosimp(s);
        } else s = out;
      }
    }
    UI.eval_cmdline1end(value, out, s);
  },
  set_locale: function () {
    UI.caseval_noautosimp('python_compat(' + (UI.python_mode ? UI.python_mode : 0) + ')');
    if (UI.langue <= 0) {
      var out = UI.caseval_noautosimp('set_language(' + -UI.langue + '); ');
      //UI.langue=1; // commente sinon la langue n'est pas toujours reconnue
    }
    if (UI.initconfigstring != '') {
      UI.caseval_noautosimp(UI.initconfigstring);
      UI.initconfigstring = ''
    }
  },
  switch_buttons: function (field, onoff) {
    if (!field || !field.firstChild) return;
    var f = field.firstChild;
    // f = f.nextSibling; // &bull; in move_buttons
    if (onoff) f.style.visibility = 'visible'; else f.style.visibility = 'hidden';
    f = f.nextSibling;
    if (onoff) f.style.visibility = 'visible'; else f.style.visibility = 'hidden';
    f = field.lastChild;
    if (onoff) f.style.visibility = 'visible'; else f.style.visibility = 'hidden';
  },
  exec: function (field, start) {
    UI.set_locale();
    var cur = field.firstChild;
    var i = 0;
    for (; cur; i++) {
      if (i >= start)
        UI.eval_level(cur);
      cur = cur.nextSibling;
    }
    if (UI.focusaftereval) UI.focused.focus();
  },
  eval_below: function (field, name, value) {
    //console.log(name+':='+value);
    UI.caseval_noautosimp('assume(' + name + '=' + value + ')');
    var cur = field.parentNode.parentNode.parentNode;
    cur = cur.nextSibling;
    //Module.print(cur.innerHTML);
    for (; cur;) {
      //console.log(cur);
      UI.eval_level(cur);
      cur = cur.nextSibling;
    }
  },
  setmathjax: function () {
    UI.usemathjax = true;
  },
  rendercomment: function (t) { // replace \n by <br>, for $$ call caseval on mathml or latex(quote())
    var res = '', i = 0, l = t.length, cas, indollar = false;
    // console.log(t,l);
    for (; i < l; ++i) {
      var ch = t.charAt(i), nxt = 0;
      if (i < l - 1) nxt = t.charAt(i + 1);
      if (ch == '$' && nxt == '$') {
        if (!UI.usemathjax) {
          var script = document.createElement("script");
          script.type = "text/javascript";
          script.src = "https://cdnjs.cloudflare.com/ajax/libs/mathjax/2.7.0/MathJax.js?config=TeX-AMS_CHTML";
          document.getElementsByTagName("head")[0].appendChild(script);
          window.setTimeout(UI.setmathjax, 400);
        }
        return t;
      }
      // console.log(i,ch,nxt);
      if (ch == '\n') {
        if (!indollar) res += '<br>';
        continue;
      }
      if (ch == '\\' && nxt == '$') {
        if (indollar) cas += nxt; else res += nxt;
        continue;
      }
      if (ch != '$') {
        if (indollar) cas += ch; else res += ch;
        continue;
      }
      if (!indollar) {
        indollar = true;
        cas = '';
        continue;
      }
      indollar = false;
      if (UI.usemathjax)
        cas = 'latex(quote(' + cas + '))';
      else
        cas = 'mathml(quote(' + cas + '),1)';
      //console.log(cas);
      cas = UI.caseval_noautosimp(cas);
      //console.log(cas);
      if (cas.charAt(0) == '"' && cas.length > 2)
        cas = cas.substr(1, cas.length - 2);
      if (UI.usemathjax)
	cas='$$'+cas+'$$';
      res += cas;
    }
    return res;
  },
  add_math: function (field) {
    UI.insert(field, "\$ \$");
    UI.moveCaret(field, -2);
  },
  add_strong: function (field) {
    UI.insert(field, "<strong></strong>");
    UI.moveCaret(field, -9);
  },
  add_em: function (field) {
    UI.insert(field, "<em></em>");
    UI.moveCaret(field, -5);
  },
  add_red: function (field) {
    UI.insert(field, "<font color='red'></font>");
    UI.moveCaret(field, -7);
  },
  add_green: function (field) {
    UI.insert(field, "<font color='green'></font>");
    UI.moveCaret(field, -7);
  },
  add_tt: function (field) {
    UI.insert(field, "<tt></tt>");
    UI.moveCaret(field, -5);
  },
  add_href: function (field) {
    UI.insert(field, "<a href=\"http://\" target=\"_blank\"></a>");
    UI.moveCaret(field, -22);
  },
  add_img: function (field) {
    UI.insert(field, "<img width=\"32\" height=\"32\" src=\"\">");
    UI.moveCaret(field, -2);
  },
  add_list: function (field) {
    UI.insert(field, "<ul>\n<li>\n<li>\n<li>\n</ul>");
    UI.moveCaretUpDown(field, -3);
  },
  add_enum: function (field) {
    UI.insert(field, "<ol>\n<li>\n<li>\n<li>\n</ol>");
    UI.moveCaretUpDown(field, -3);
  },
  add_h1: function (field) {
    UI.insert(field, "<h1></h1>");
    UI.moveCaret(field, -5);
  },
  add_h2: function (field) {
    UI.insert(field, "<h2></h2>");
    UI.moveCaret(field, -5);
  },
  addcommentafter: function (field, comment, s) {
    var f = field.parentNode.parentNode.parentNode;
    var p = f.parentNode;
    var add = comment ? UI.addcomment('') : UI.addinput(s, '', '');
    var tr = document.createElement("TABLE");
    tr.innerHTML += add;
    if (f.nextSibling) {
      f = f.nextSibling;
      p.insertBefore(tr.firstChild, f);
      f = f.previousSibling;
    }
    else {
      f = p.appendChild(tr.firstChild);
    }
    if (comment)
      f.firstChild.firstChild.nextSibling.nextSibling.firstChild.nextSibling.click();
    else
      f.firstChild.firstChild.nextSibling.nextSibling.firstChild.focus();
  },
  addcomment: function (text) {
    UI.show_history123();
    var s = UI.move_buttons(!UI.qa);
    var t = text.length >= 3 ? text.substr(3, text.length - 3) : text;
    s += '<td colspan="2"><span style="display:none"><textarea title="';
    s += UI.langue == -1 ? 'Ctrl-Enter ou Ok pour valider ce commentaire' : 'Ctrl-Enter or Ok will update this comment';
    s += '" onfocus="UI.focused=this" onkeypress="UI.ckenter_comment(event,this)" row="5" cols="60">' + t + '</textarea>';
    s += '<button class="bouton" title="';
    s += UI.langue == -1 ? 'Valide ce commentaire' : 'Update comment';
    s += '" onclick="UI.editcomment_end(this,true)">Ok</button>';
    s += '<br><button onmousedown="event.preventDefault()" title="';
    s += UI.langue == -1 ? 'Ajoute $ $ pour ins&eacute;rer des maths' : 'Add $ $ to insert maths';
    s += '" class="bouton" onclick="UI.add_math(UI.focused)">math</button>';
    s += '<button onmousedown="event.preventDefault()" title="'
    s += UI.langue == -1 ? 'Passe en police fixe' : 'Fixed size police';
    s += '" class="bouton" onclick="UI.add_tt(UI.focused)"><tt>Abc</tt></button>';
    s += '<button title="';
    s += UI.langue == -1 ? 'Passe en gras' : 'Boldface';
    s += '" onmousedown="event.preventDefault()"  class="bouton" onclick="UI.add_strong(UI.focused)"><strong>Abc</strong></button>';
    s += '<button title="';
    s += UI.langue == -1 ? 'Passe en italique' : 'italics';
    s += '" onmousedown="event.preventDefault()" class="bouton" onclick="UI.add_em(UI.focused)"><em>Abc</em></button>';
    s += '<button title="Red/Rouge" onmousedown="event.preventDefault()" class="bouton" onclick="UI.add_red(UI.focused)"><font color="red">Abc</font></button>';
    s += '<button title="Green/Vert" onmousedown="event.preventDefault()" class="bouton" onclick="UI.add_green(UI.focused)"><font color="green">Abc</font></button>';
    s += '<button onmousedown="event.preventDefault()" title="';
    s += UI.langue == -1 ? 'Ins&egrave;re une liste' : 'List insertion';
    s += '" class="bouton" onclick="UI.add_list(UI.focused)">list</button>';
    s += '<button onmousedown="event.preventDefault()" title="';
    s += UI.langue == -1 ? 'Ins&egrave;re une listenum&eacute;rot&eacute;e' : 'Numbered list insertion';
    s += '" class="bouton" onclick="UI.add_enum(UI.focused)">num</button>';
    s += '<button onmousedown="event.preventDefault()" title="';
    s += UI.langue == -1 ? 'Ins&egrave;re un lien' : 'Link insertion';
    s += '" class="bouton" onclick="UI.add_href(UI.focused)">link</button>';
    s += '<button onmousedown="event.preventDefault()" title="';
    s += UI.langue == -1 ? 'Ins&egrave;re une image' : 'Image insertion';
    s += '" class="bouton" onclick="UI.add_img(UI.focused)">img</button>';
    s += '<button style="display:none" onmousedown="event.preventDefault()" class="bouton" onclick="UI.add_h2(UI.focused)">sous</button>';
    s += UI.langue == -1 ? '<button onmousedown="event.preventDefault()" title="Ins&egrave;re un titre" class="bouton" onclick="UI.add_h1(UI.focused)">titre</button>' : '<button onmousedown="event.preventDefault()" title="Insert title" class="bouton" onclick="UI.add_h1(UI.focused)">title</button>';
    s += '</span><span onclick="UI.editcomment1(this)">' + UI.rendercomment(t) + '</span></td>';
    s += '<td> <button title="';
    s += UI.langue == -1 ? 'Editer ce commentaire' : 'Edit this comment';
    s += '" style="width:20px;height:30px;" onclick="UI.editcomment2(this);">&#x270e;</td>';
    s += UI.erase_button(!UI.qa);
    return s;
  },
  editcomment1: function (field) {
    var prev = field.parentNode, prevff = prev.firstChild.firstChild;
    //console.log(prevff);
    var pos = prevff.value.search('<a href="');
    if (pos >= 0 && pos < prevff.value.length)
      return;
    field.style.display = 'none';
    prev.firstChild.style.display = 'inline';
    // console.log(prev.firstChild.firstChild.innerHTML);
    UI.focused = prevff;
    UI.resizetextarea(UI.focused);
    UI.focused.focus();
    prev.nextSibling.style.display = 'none';
  },
  editcomment2: function (field) {
    var prev = field.parentNode.previousSibling;
    prev.firstChild.style.display = 'inline';
    UI.focused = prev.firstChild.firstChild;
    UI.resizetextarea(UI.focused);
    UI.focused.focus();
    prev.lastChild.style.display = 'none';
    field.parentNode.style.display = 'none';
  },
  editcomment_end: function (field, comment) {
    var prev = field.previousSibling;
    var s = prev.value;
    var par = field.parentNode;
    //console.log(par.parentNode.parentNode.lastChild);
    if (s.length >= 3 && s.substr(0, 3) == '///') {
      comment = false;
      s = s.substr(3, s.length - 3);
    }
    if (s.length == 0) {
      s = '0';
      comment = false;
    }
    if (!comment) {
      UI.addcommentafter(par, false, s);
    }
    par.style.display = 'none';
    par.nextSibling.style.display = 'inline';
    if (comment) {
      par.nextSibling.innerHTML = UI.rendercomment(s);
      if (UI.usemathjax)
        MathJax.Hub.Queue(["Typeset", MathJax.Hub, par.nextSibling]);
    }
    par.parentNode.nextSibling.style.display = 'inline';
    UI.link(0);
    if (!comment)
      par.parentNode.parentNode.lastChild.firstChild.click();
    else
      UI.focused = cmentree;
  },
  eval: function (text, textin) {
    UI.set_locale();
    var out = UI.caseval(text);
    //console.log(text,out);
    var s = ' ';
    var isstr = out[0] == '"';
    if (out.substr(1, 4) == '<svg' || out.substr(0, 5) == 'gl3d ' || out.substr(0, 5) == 'gr2d(') {
      // Module.print(text+' -> Done');
      //console.log(out);
      s = out;
      out = 'Done_graphic';
    }
    else {
      // Module.print(text+' -> '+out);
      if (out[0] == '"')
        s = 'text ' + out;
      else {
	if (out.substr(0, 10) == 'GIAC_ERROR')
	  s=' '+out+' ';
	else {
          if (UI.prettyprint ) {
            if (UI.usemathjax)
              s = UI.caseval_noautosimp('latex(quote(' + out + '))');
            else
              s = UI.caseval_noautosimp('mathml(quote(' + out + '),1)');
          } else s = out;
	}
      }
    }
    s = UI.addinput(textin, out, s);
    return s;
  },
  render_canvas: function (field) {
    // return; // does not work,
    var n = field.id;
    if (n && n.length > 5 && n.substr(0, 5) == 'gr2d_') {
      //console.log(field.parentNode.innerHTML);
      UI.turtle_draw(n, field.nextSibling.value);
    }
    if (n && n.length > 5 && n.substr(0, 5) == 'gl3d_') {
      Module.print(n);
      var n3d = n.substr(5, n.length - 5);
      //Module.print(n3d);
      //Module.canvas=$id(n);
      UI.giac_renderer(n3d);
      //Module.canvas=$id('canvas');
      return;
    }
    var f = field.firstChild;
    for (; f; f = f.nextSibling) {
      UI.render_canvas(f);
    }
  },
  search: function (field, cmd) {
    if (!field) return;
    field = field.parentNode;
    if (field) {
      if (field) {
        if (field.parentNode)
          field = field.parentNode;
        if (field) {
          field = field.firstChild;
          if (field) {
            var cm = field.nextSibling.CodeMirror;
            if (field.id == 'entree') cm = cmentree;
            if (field.nextSibling && field.nextSibling.id == 'entree') cm = cmentree;
            if (cm) {
              if (cmd == 0) {
                CodeMirror.commands.find(cm);
                return;
              }
              if (cmd == 1) {
                CodeMirror.commands.findNext(cm);
                return;
              }
              if (cmd == 2) {
                CodeMirror.commands.findPrev(cm);
                return;
              }
              if (cmd == 3) {
                CodeMirror.commands.replace(cm);
                return;
              }
              if (cmd == 4) {
                CodeMirror.commands.replaceAll(cm);
                return;
              }
              if (cmd == 5) {
                CodeMirror.commands.jumpToLine(cm);
                return;
              }
              if (cmd == 6) {
                cm.undo();
                return;
              }
              if (cmd == 7) {
                cm.redo();
                return;
              }
              if (cmd == -1) {
                UI.completion(cm);
                return;
              }
            }
          }
        }
      }
    }
  },
  evallevel: function (field, eval) {
    if (!field) return;
    field = field.parentNode;
    if (field) {
      field.style.display = 'none'; // hide ok button
      if (field) {
        if (field.parentNode)
          field = field.parentNode;
        if (field) {
          field = field.firstChild;
          if (field) {
            if (field.nextSibling.CodeMirror) { // convert codemirror back
              var cm = field.nextSibling;
              cm.CodeMirror.toTextArea();
            }
            if (eval) UI.reeval(field, '', false); else UI.link(0);
          }
        }
      }
    }
  },
  reeval: function (field, postcmd, focusnextsibling) {
    var field_ = field;
    // field=field.previousSibling;
    UI.set_config_width();
    if (field.type != 'textarea') {
      var t = field.getTextArea();
      t.value = field.getValue();
      field = t;
    }
    UI.set_editline(field, false);
    var s = field.value;
    //console.log(s);
    var par = field.parentNode;
    par = par.parentNode;
    if (s.length && postcmd.length && postcmd[0] == ';') {
      var ss = s.length - 1;
      for (; ss >= 0; ss--) {
        if (s[ss] != ' ' && s[ss] != '\n')
          break;
      }
      if (ss >= 0 && s[ss] == ';')
        postcmd = postcmd.substr(1, postcmd.length - 1);
    }
    s = UI.eval(s + postcmd, s);
    par.innerHTML = s;
    UI.render_canvas(par);
    UI.link(0);
    if (UI.prettyprint && UI.usemathjax && UI.histcount > 0) {
      //console.log('"hist'+(UI.histcount-1)+'"');
      MathJax.Hub.Queue(["Typeset", MathJax.Hub, '"hist' + (UI.histcount - 1) + '"']);
    }
    if (UI.focusaftereval) {
      if (focusnextsibling) {
        par = par.parentNode.nextSibling;
        if (par == null) cmentree.focus();
        else {
          par = par.firstChild.firstChild.nextSibling.nextSibling;
          par = par.firstChild;
          if (par.style.display == 'none') { // commentaire
            par = par.nextSibling;
            if (par.CodeMirror) {
              par = par.CodeMirror;
              UI.focused = par;
              par.focus();
            }
            else
              par.click();
          }
          else {
            par.focus();
          }
        }
      }
      else
        cmentree.focus();
    }
  },
  eval_level: function (field) {
    // ? use cur.nodeType instead of search?
    var s = field.innerHTML;
    var pos = s.search("<textarea");
    if (pos < 0 || pos >= s.length) {
      pos = s.search("<form");
      if (pos > 0 && pos < s.length) {
        var level = field.firstChild;
        var cur = level.firstChild;
        cur = UI.skip_buttons(cur);
        //console.log(cur);
        cur = cur.firstChild;
        //Module.print(cur.name.value+':='+cur.rangename.value);
        UI.caseval_noautosimp('assume('+cur.name.value + '=' + cur.rangename.value+')');
        var s = UI.curseurhtml(cur.name.value, cur.minname.value, cur.maxname.value, cur.stepname.value, cur.valname.value);
        level.innerHTML = s;
      }
      return;
    }
    var cur = field.firstChild;
    cur = cur.firstChild;
    cur = UI.skip_buttons(cur);
    cur = cur.firstChild;
    var tst = cur.firstChild;
    if (tst != undefined && tst.nextSibling != undefined) { // commentaire
      tst.nextSibling.click();
      return;
    }
    s = cur.value;
    s = UI.eval(s, s);
    field.innerHTML = s;
    UI.render_canvas(field);
    if (UI.prettyprint && UI.usemathjax) {
      //console.log(s);
      MathJax.Hub.Queue(["Typeset", MathJax.Hub, field]);
    }
  },
  before: function (field) {
    var s = '';
    while ((field = field.previousSibling)) {
      s += field.innerHTML;
    }
    return s;
  },
  after: function (field) {
    var s = '';
    while ((field = field.nextSibling)) {
      s += field.innerHTML;
    }
    return s;
  },
  not_empty:function(txt){
    var s=txt.length;
    //console.log(s);
    for (var i=0;i<s;++i){
      if (txt[i]!=' ' && txt[i]!='\n')
	return true;
    }
    return false;
  },
  prepare_cm: function (txt, h, cm) {
    var pos = txt.selectionStart;
    var i, s = txt.value, l = 0, c = 0;
    for (i = 0; i < pos; ++i) {
      if (s[i] == '\n') {
        l++;
        c = 0;
      }
      else c++;
    }
    cm.setCursor({line: l, ch: c});
    cm.refresh();
    UI.setoption_mode(cm);
    //cm.setSize(null,h+20); // commented (auto-resize)
    cm.on("focus", function (cm) {
      UI.set_focused(cm);
      UI.set_editline(cm, true); // insure OK is visible on mobile
      UI.set_config_width();
    });
    cm.on("blur", function (cmf) {
      //cmf.getTextArea().value=cmf.getValue();
      //UI.resizetextarea(cmf.getTextArea());
      if (cmf.getSelection().length > 0) {
        UI.selection = cmf.getSelection();
      }
      //UI.set_editline(cm,false);
      //UI.set_config_width();
      //cmf.toTextArea();
    });
    // cm.setOption("inputStyle", "textarea");
    cm.setOption("extraKeys", {
      Enter: function (cm) {
        // Enter inserts \n except on empty line (two Enter without indent reevals)
        var start = cm.getCursor('from');
        var end = cm.getCursor('to');
        if (end.line != start.line || end.ch != start.ch || ((start.line > 0 || start.ch > 0) && UI.not_empty(cm.getLine(end.line)))) {
          UI.insert(cm, '\n');
          return;
        }
        var txt = cm.getTextArea();
        cm.toTextArea();
        UI.reeval(txt, '', true);
      },
      "Ctrl-Enter": function (cm) {
        var txt = cm.getTextArea();
        cm.toTextArea();
        UI.reeval(txt, '', true);
      },
      Backspace: function (cm) {
        UI.backspace(cm);
      },
      F1: function (cm) {
        UI.completion(cm);
      },
      "Ctrl-Space": function (cm) {
        UI.completion(cm);
      },
      Tab: function (cm) {
        UI.indent_or_complete(cm);
        //cm.toTextArea();
      }
    });
  },
  count_newline: function (text) {
    var k = 0, r = 0;
    for (; k < text.length; ++k) {
      if (text.charCodeAt(k) == 10)
        ++r;
    }
    return r;
  },
  addinput: function (textin, textout, mathmlout) {
    $id('startup_restore').style.display = 'none'
    //console.log(textin,textout,mathmlout);
    if (mathmlout.length >= 5 && mathmlout.substr(0, 5) == 'gl3d ') {
      $id('table_3d').style.display = 'inherit';
    }
    if (textin.length == 0 || (textin.length >= 3 && textin.substr(0, 3) == '///')) return UI.addcomment(textin);
    UI.show_history123();
    // $id('mathoutput').style.listStyleType = 'none';
    var is_svg = mathmlout.substr(1, 4) == '<svg';
    var is_3d = mathmlout.substr(0, 5) == 'gl3d ';
    var is_gr2d = mathmlout.substr(0, 5) == 'gr2d(';
    var s = UI.move_buttons(!UI.qa);
    var delbut = false;
    if (textin.length && textin.charCodeAt(0) == 63)
      s += '<td colspan="2">' + UI.renderhelp(textout, textin.substr(1, textin.length - 1)) + '</td><td>';
    else {
      if (UI.qa)
        s += '<td>';
      else
        s += '<td colspan=2>';
      s += '<textarea class="historyinput" ';
      if (is_svg && UI.qa)
        s += 'rows=8 style="font-size:large"';
      else
      //s += 'rows='+(UI.count_newline(textin)+1) +' style="font-size:large"';
        s += 'style="height:' + (20 + 16 * UI.count_newline(textin)) + 'px; font-size:large"';
      s += ' title="Enter: saut de ligne, Ctrl-Enter: eval" onkeypress="UI.ckenter(event,this)" onblur="UI.updatelevel(this);" onfocus="if (UI.usecm) {var h=offsetHeight;UI.history_cm=CodeMirror.fromTextArea(this,{ matchBrackets: true,lineNumbers:true,viewportMargin: Infinity}); UI.history_cm.setCursor({line:0,ch:selectionStart});UI.history_cm.options.indentUnit=UI.python_mode?UI.python_indent:2; UI.prepare_cm(this,h,UI.history_cm);UI.changefontsize(UI.history_cm,16); UI.set_focused(UI.history_cm);} else UI.set_focused(this); UI.set_editline(this,true); UI.set_config_width(); parentNode.scrollIntoView();UI.moveCaret(UI.focused,1);UI.moveCaret(UI.focused,-1);" onselect="if (UI.getsel(this).length>0) UI.selection=UI.getsel(this);">' + textin + '</textarea>';
      s += '<span style="display:none">';
      s += '<button class="bouton" onmousedown="event.preventDefault()" onclick="UI.evallevel(this,true)" title="';
      s += UI.langue == -1 ? 'R&eacute;evaluer le niveau (Ctrl-Enter)' : 'Reeval level (Ctrl-Enter)';
      s += '">&nbsp;Ok&nbsp;&nbsp;</button>';
      s += '<button class="bouton" onmousedown="event.preventDefault()" onClick="UI.search(this,-1);" title="';
      s += UI.langue == -1 ? 'Donne une aide courte et quelques exemples d\'utilisation d\'une commande.' : 'Short help and examples on a command';
      s += '">&nbsp;?&nbsp;</button>';
      s += '<button class="bouton" onmousedown="event.preventDefault()" onClick="UI.move_caret_or_focus(UI.focused,-1)" title="cursor ←">&nbsp;←&nbsp;</button>'
      s += '<button class="bouton" onmousedown="event.preventDefault()" onClick="UI.move_caret_or_focus(UI.focused,1)" title="cursor →">&nbsp;→&nbsp;</button>'
      s += '<button class="bouton" onmousedown="event.preventDefault()" onClick="UI.moveCaretUpDown(UI.focused,-1)" title="cursor ↑">↑&nbsp;</button>'
      s += '<button class="bouton" onmousedown="event.preventDefault()" onClick="UI.moveCaretUpDown(UI.focused,1)" title="cursor ↓">&nbsp;↓</button>'
      s += '<button class="bouton" onmousedown="event.preventDefault()" onClick="UI.indentline(UI.focused)" title="Indent">→|</button>'
      s += '<button class="bouton" onmousedown="event.preventDefault()" onClick="UI.backspace(UI.focused)" title="';
      s += UI.langue == -1 ? 'Efface' : 'Erase';
      s += '">&#x232b;</button>'
      if (UI.usecm) {
        //s += '<br>';
        s += '<button class="bouton" onmousedown="event.preventDefault()" onclick="UI.search(this,6)" title="';
        s += UI.langue == -1 ? 'Annuler' : 'Undo';
        s += '"><img width="16" height="16" src="undo.png" alt="Annuler" align="center"></button>';
        s += '<button class="bouton" onmousedown="event.preventDefault()" onclick="UI.search(this,7)" title="';
        s += UI.langue == -1 ? 'Refaire' : 'Redo';
        s += '"><img width="16" height="16" src="redo.png" alt="Redo" align="center"></button>';
        s += '<button class="bouton" onmousedown="event.preventDefault()" onclick="UI.search(this,0)" title="';
        s += UI.langue == -1 ? 'Rechercher' : 'Find';
        s += '">find</button><button class="bouton" onmousedown="event.preventDefault()" onclick="UI.search(this,1)" title="';
        s += UI.langue == -1 ? 'Rechercher suivant' : 'Find next';
        s += '">&#x21D2;</button><button class="bouton" onmousedown="event.preventDefault()" onclick="UI.search(this,2)" title="';
        s += UI.langue == -1 ? 'Rechercher pr&eacute;c&eacute;dent' : 'Find previous';
        s += '">&#x21D0;</button>';
        s += '<button class="bouton" onmousedown="event.preventDefault()" onclick="UI.search(this,3)" title="';
        s += UI.langue == -1 ? 'Remplacer' : 'Replace';
        s += '">rep</button><button class="bouton" onmousedown="event.preventDefault()" onclick="UI.search(this,5)" title="';
        s += UI.langue == -1 ? 'Aller ligne' : 'Go line';
        s += '">go</button>';
      }
      s += '<button class="bouton" onmousedown="event.preventDefault()" onclick="UI.evallevel(this,false)" title="';
      s += UI.langue == -1 ? 'Abandonner' : 'Cancel';
      s += '">&nbsp;X</button>';
      s += '</span>';
      if (UI.qa) s += '</td>';
      if (is_svg || is_3d || is_gr2d) {
        if (UI.qa) s += '<td>'; else s += '<br>';
        if (is_svg)
          s += '<div style="text-align:center">' + mathmlout.substr(1, mathmlout.length - 2) + '</div></td>';
        else {
          if (is_gr2d) {
            var gr2ds = 'gr2d_' + UI.gr2d_ncanvas;
            s += "<div style='text-align:center'> <canvas id='" + gr2ds + "' width=" + UI.canvas_w + " height=" + UI.canvas_h + "> </canvas><textarea style='display:none'>" + mathmlout + "</textarea></div></td>";
            UI.gr2d_ncanvas++;
          }
          else {
            var n3d = mathmlout.substr(5, mathmlout.length - 5);
            //Module.print(n3d);
            if (0)
              s += '<div style="text-align:center"> 3d </div></td>';
            else s += '<div style="text-align:center"> <canvas id="gl3d_' + n3d + '" onmousedown="UI.canvas_pushed=true;UI.canvas_lastx=event.clientX; UI.canvas_lasty=event.clientY;" onmouseup="UI.canvas_pushed=false;" onmousemove="UI.canvas_mousemove(event,' + n3d + ')" width=' + 400 + ' height=' + 250 + '></canvas></div></td>';
            //Module.print(s);
          }
        }
        s += '<td>';
        if (!is_gr2d) {
          s += '<button  style="width:32px;height:30px;" onclick="UI.zoom(this,1.414)">out</button><br>';
          s += '<button  style="width:32px;height:30px;" onclick="UI.zoom(this.previousSibling,0.707)">in</button><br>';
        }
        if (is_svg) s += '<button style="width:32px;height:30px;" onclick="UI.savesvg(parentNode.previousSibling)">sav</button><br>';
        s += '<br><button style="width:32px;height:30px;" onclick=\'UI.erase(this)\' title="';
        s += UI.langue == -1 ? 'placer ce niveau dans la corbeille' : 'throw this level in the trash';
        s += '">&#x232b;</button>';
        delbut = true;
        s += '</td>';
      }
      else {
        if (UI.qa) s += '<td>';
        s += '<div style="color:blue; text-align:center" title="';
        s += UI.langue == -1 ? 'Double clic: voir en mode texte (utile pour copier-coller)' : 'Double clic: view answer in text mode, useful for copy/paste';
        s += '" class="outdiv" ondblclick="nextSibling.style.display=\'inherit\';this.nextSibling.select();UI.selection=nextSibling.value;this.style.display=\'none\';this.nextSibling.nextSibling.style.display=\'inherit\';" id="hist' + UI.histcount + '">';
        UI.histcount++;
        //console.log('addinput',mathmlout);
        if (mathmlout.length > 7 && mathmlout.substr(0, 6) == 'text "') {
          s += '<textarea class="historyinput" style="height:' + (20 + 16 * UI.count_newline(mathmlout)) + 'px; font-size:large">' + mathmlout.substr(6, mathmlout.length - 7).replace(/\"\"/g, '\"') + '</textarea>';
        }
        else {
          if (UI.prettyprint) {
            if (UI.usemathjax)
              s += '$$' + mathmlout.substr(1, mathmlout.length - 2) + '$$';
            else
              s += mathmlout.substr(1, mathmlout.length - 2);
          } else s += mathmlout;
        }
        s += '</div>';
        s += '<textarea class="outdiv" onfocus="UI.set_focused(this)" onselect="if (UI.getsel(this).length>0) UI.selection=UI.getsel(this);" style="display:none">' + textout + '</textarea>';
        s += '<button style="display:none" onclick="previousSibling.previousSibling.style.display=\'block\'; previousSibling.style.display=\'none\';this.style.display=\'none\'">cancel</button>';
        s += '</td><td>';
        s += '<button style="width:20px;height:30px;" onclick="UI.addcommentafter(this,true,\'\')" oncontextmenu="UI.addcommentafter(this,false,\'0\'); return false;" title="';
        s += UI.langue == -1 ? 'Ajouter un commentaire (ou clic droit pour ajouter une ligne de commande)' : 'Add a comment (or right click to add a commandline)';
        s += '" >&#x270e;</button>';
        //s += '<button style="width:20px;height:30px;" onclick=\'UI.insert(UI.focused,decodeURIComponent("evalf('+encodeURIComponent(textin)+')"))\' title="Valeur approch&eacute;e" style="color:blue">~</button>';
        //s += '<button style="width:20px;height:30px;" onclick=\'UI.insert(UI.focused,decodeURIComponent("latex('+encodeURIComponent(textin)+')"))\' title="convertir en LaTeX" style="color:blue">TeX</button>';
        if (!UI.qa) {
          s += '<button style="height:25px;" onclick=\'UI.erase(this)\' title="';
          s += UI.langue == -1 ? 'placer ce niveau dans la corbeille' : 'throw this level in the trash';
          s += '">del</button>';
          delbut = true;
        }
        s += '</td>';
      }
    }
    if (delbut) s += '</tr>'; else s += UI.erase_button(!UI.qa);
    // console.log(s);
    return s;
  },
  giac_renderer: function (text) {
    var gr = Module.cwrap('_ZN4giac13giac_rendererEPKc', 'number', ['string']);
    gr(text);
    var keyboardListeningElement = Module['keyboardListeningElement'] || document;
    keyboardListeningElement.removeEventListener("keydown", SDL.receiveEvent);
    keyboardListeningElement.removeEventListener("keyup", SDL.receiveEvent);
    keyboardListeningElement.removeEventListener("keypress", SDL.receiveEvent);
  },
  xcascmd: ["!","!=","#","$","%","%/","%/","%{%}","&&","&*","&^","'","()","*","*=","+","+&","+=","+infinity","-","-<","-=","->","-infinity",".*",".+",".-","./",".^","/%","/=",":=","<","<=","=","=<","==","=>",">",">=","?","?","@","@@","ACOSH","ACOT","ACSC","ASEC","ASIN","ASINH","ATAN","ATANH","Airy_Ai","Airy_Bi","Archive","BesselJ","BesselY","Beta","BlockDiagonal","COND","COS","COSH","COT","CSC","CST","Celsius2Fahrenheit","Ci","Circle","ClrDraw","ClrGraph","ClrIO","Col","CopyVar","CyclePic","DIGITS","DOM_COMPLEX","DOM_FLOAT","DOM_FUNC","DOM_IDENT","DOM_INT","DOM_LIST","DOM_RAT","DOM_STRING","DOM_SYMBOLIC","DOM_int","DelFold","DelVar","Det","Dialog","Digits","Dirac","Disp","DispG","DispHome","DrawFunc","DrawInv","DrawParm","DrawPol","DrawSlp","DropDown","DrwCtour","ERROR","EXP","Ei","EndDlog","FALSE","Factor","Fahrenheit2Celsius","False","Fill","GF","Gamma","Gcd","GetFold","Graph","Heaviside","IFTE","Input","InputStr","Int","Inverse","JordanBlock","LN","LQ","LSQ","LU","LambertW","Li","Line","LineHorz","LineTan","LineVert","NORMALD","NewFold","NewPic","Nullspace","Output","Ox_2d_unit_vector","Ox_3d_unit_vector","Oy_2d_unit_vector","Oy_3d_unit_vector","Oz_3d_unit_vector","Pause","Phi","Pi","PopUp","Psi","QR","Quo","REDIM","REPLACE","RandSeed","RclPic","Rem","Request","Resultant","Row","RplcPic","Rref","SCALE","SCALEADD","SCHUR","SIN","SVD","SVL","SWAPCOL","SWAPROW","SetFold","Si","SortA","SortD","StoPic","Store","TAN","TRUE","TeX","Text","Title","True","UTPC","UTPF","UTPN","UTPT","Unarchiv","VARS","VAS","VAS_positive","WAIT","Zeta","[..]","^","_(cm/s)","_(ft/s)","_(m/s)","_(m/s^2)","_(rad/s)","_(rad/s^2)","_(tr/min)","_(tr/s)","_A","_Angstrom","_Bq","_Btu","_Ci","_F","_F_","_Fdy","_G_","_Gal","_Gy","_H","_Hz","_I0_","_J","_K","_Kcal","_MHz","_MW","_MeV","_N","_NA_","_Ohm","_P","_PSun_","_Pa","_R","_REarth_","_RSun_","_R_","_Rankine","_Rinfinity_","_S","_St","_StdP_","_StdT_","_Sv","_T","_V","_Vm_","_W","_Wb","_Wh","_a","_a0_","_acre","_alpha_","_angl_","_arcmin","_arcs","_atm","_au","_b","_bar","_bbl","_bblep","_bu","_buUS","_c3_","_c_","_cal","_cd","_chain","_cm","_cm^2","_cm^3","_ct","_cu","_d","_dB","_deg","_degreeF","_dyn","_eV","_epsilon0_","_epsilon0q_","_epsilonox_","_epsilonsi_","_erg","_f0_","_fath","_fbm","_fc","_fermi","_flam","_fm","_ft","_ft*lb","_ftUS","_ft^2","_ft^3","_g","_g_","_ga","_galC","_galUK","_galUS","_gf","_gmol","_gon","_grad","_grain","_h","_h_","_ha","_hbar_","_hp","_in","_inH20","_inHg","_in^2","_in^3","_j","_kWh","_k_","_kg","_kip","_km","_km^2","_knot","_kph","_kq_","_l","_lam","_lambda0_","_lambdac_","_lb","_lbf","_lbmol","_lbt","_lep","_liqpt","_lm","_lx","_lyr","_m","_mEarth_","_m^2","_m^3","_me_","_mho","_miUS","_miUS^2","_mi^2","_mil","_mile","_mille","_ml","_mm","_mmHg","_mn","_mol","_mp_","_mph","_mpme_","_mu0_","_muB_","_muN_","_oz","_ozUK","_ozfl","_ozt","_pc","_pdl","_ph","_phi_","_pk","_psi","_ptUK","_q_","_qe_","_qepsilon0_","_qme_","_qt","_rad","_rad_","_rd","_rem","_rod","_rpm","_s","_s","_sb","_sd_","_sigma_","_slug","_sr","_st","_syr_","_t","_tbsp","_tec","_tep","_tex","_therm","_ton","_tonUK","_torr","_tr","_tsp","_twopi_","_u","_yd","_yd^2","_yd^3","_yr","_µ","_Âµ","a2q","abcuv","about","abs","abscissa","accumulate_head_tail","acos","acos2asin","acos2atan","acosh","acot","acsc","acyclic","add","add_arc","add_edge","add_vertex","additionally","adjacency_matrix","adjoint_matrix","affix","algsubs","algvar","all_trig_solutions","allpairs_distance","alog10","alors","altitude","and","angle","angle_radian","angleat","angleatraw","animate","animate3d","animation","ans","antiprism_graph","append","apply","approx","approx_mode","arc","arcLen","arccos","arccosh","archive","arclen","arcsin","arcsinh","arctan","arctanh","area","areaat","areaatraw","areaplot","arg","args","array","arrivals","articulation_points","as_function_of","asc","asec","asin","asin2acos","asin2atan","asinh","assert","assign","assign_edge_weights","assume","at","atan","atan2acos","atan2asin","atanh","atrig2ln","augment","auto_correlation","autosimplify","avance","avgRC","axes","back","backquote","backward","baisse_crayon","bandwidth","bar_plot","bartlett_hann_window","barycenter","base","basis","batons","begin","bellman_ford","bernoulli","besselJ","besselY","betad","betad_cdf","betad_icdf","betavariate","bezier","bezout_entiers","biconnected_components","binomial","binomial_cdf","binomial_icdf","bins","bipartite","bipartite_matching","bisection_solver","bisector","bit_depth","bitand","bitor","bitxor","black","blackman_harris_window","blackman_window","bloc","blockmatrix","blue","bohman_window","border","boxwhisker","break","breakpoint","brent_solver","bvpsolve","by","c1oc2","c1op2","cFactor","cSolve","cZeros","cache_tortue","camembert","canonical_form","canonical_labeling","cap","cap_flat_line","cap_round_line","cap_square_line","cartesian_product","cas_setup","case","cat","catch","cauchy","cauchy_cdf","cauchy_icdf","cauchyd","cauchyd_cdf","cauchyd_icdf","cd","cdf","ceil","ceiling","center","center2interval","centered_cube","centered_tetrahedron","cfactor","cfsolve","changebase","channel_data","channels","char","charpoly","chinrem","chisquare","chisquare_cdf","chisquare_icdf","chisquared","chisquared_cdf","chisquared_icdf","chisquaret","choice","cholesky","choosebox","chr","chrem","chromatic_index","chromatic_number","chromatic_polynomial","circle","circumcircle","classes","clear","click","clique_cover","clique_cover_number","clique_number","clique_stats","close","clustering_coefficient","coeff","coeffs","col","colDim","colNorm","colSwap","coldim","collect","colnorm","color","colspace","colswap","comDenom","comb","combine","comment","common_perpendicular","companion","compare","complete_binary_tree","complete_graph","complete_kary_tree","complex","complex_mode","complex_variables","complexroot","concat","cond","cone","confrac","conic","conj","conjugate_equation","conjugate_gradient","connected","connected_components","cont","contains","content","continue","contourplot","contract_edge","convert","convertir","convex","convexhull","convolution","coordinates","copy","correlation","cos","cos2sintan","cosh","cosine_window","cot","cote","count","count_eq","count_inf","count_sup","courbe_parametrique","courbe_polaire","covariance","covariance_correlation","cpartfrac","crationalroot","crayon","createwav","cross","crossP","cross_correlation","cross_point","cross_ratio","crossproduct","csc","csolve","csv2gen","cube","cumSum","cumsum","cumulated_frequencies","curl","current_sheet","curvature","curve","cyan","cycle2perm","cycle_graph","cycleinv","cycles2permu","cyclotomic","cylinder","dash_line","dashdot_line","dashdotdot_line","dayofweek","dayofweek","de","deSolve","debug","debut_enregistrement","default","degree","degree_sequence","del","delcols","delete_arc","delete_edge","delete_vertex","delrows","deltalist","denom","densityplot","departures","derive","deriver","desolve","dessine_tortue","det","det_minor","developper","developper_transcendant","dfc","dfc2f","diag","diff","digraph","dijkstra","dim","directed","discard_edge_attribute","discard_graph_attribute","discard_vertex_attribute","disjoint_union","display","disque","disque_centre","distance","distance2","distanceat","distanceatraw","div","divergence","divide","divis","division_point","divisors","divmod","divpc","dnewton_solver","do","dodecahedron","domain","dot","dotP","dot_paper","dotprod","double","draw_arc","draw_circle","draw_graph","draw_line","draw_pixel","draw_polygon","draw_rectangle","droit","droite_tangente","dsolve","duration","e","e2r","ecart_type","ecart_type_population","ecm_factor","ecris","edge_connectivity","edges","efface","egcd","egv","egvl","eigVc","eigVl","eigenvals","eigenvalues","eigenvectors","eigenvects","element","elif","eliminate","ellipse","else","end","end_for","end_if","end_while","entry","envelope","epaisseur","epaisseur_ligne_1","epaisseur_ligne_2","epaisseur_ligne_3","epaisseur_ligne_4","epaisseur_ligne_5","epaisseur_ligne_6","epaisseur_ligne_7","epaisseur_point_1","epaisseur_point_2","epaisseur_point_3","epaisseur_point_4","epaisseur_point_5","epaisseur_point_6","epaisseur_point_7","epsilon","epsilon2zero","equal","equal2diff","equal2list","equation","equilateral_triangle","erase","erase3d","erf","erfc","error","est_permu","et","euler","euler_gamma","euler_lagrange","eval","eval_level","evala","evalb","evalc","evalf","evalm","even","evolute","exact","exbisector","excircle","execute","exp","exp2list","exp2pow","exp2trig","expand","expexpand","expln","exponential","exponential_cdf","exponential_icdf","exponential_regression","exponential_regression_plot","exponentiald","exponentiald_cdf","exponentiald_icdf","export_graph","expovariate","expr","expression","extend","extract_measure","extrema","ezgcd","f2nd","fMax","fMin","fPart","faces","facteurs_premiers","factor","factor_xn","factorial","factoriser","factoriser_entier","factoriser_sur_C","factors","fadeev","faire","false","falsepos_solver","fclose","fcoeff","fdistrib","feuille","ffaire","ffonction","fft","ffunction","fi","fieldplot","filled","fin_enregistrement","find","findhelp","fisher","fisher_cdf","fisher_icdf","fisherd","fisherd_cdf","fisherd_icdf","fitdistr","flatten","float","float2rational","floor","flow_polynomial","fmod","foldl","foldr","fonction	","fonction_derivee","fopen","for","format","forward","fourier_an","fourier_bn","fourier_cn","fpour","fprint","frac","fracmod","frame_2d","frame_3d","frames","frequencies","frobenius_norm","from","froot","fsi","fsi","fsolve","ftantque","fullparfrac","func","funcplot","function","function_diff","fxnd","gammad","gammad_cdf","gammad_icdf","gammavariate","gauche","gauss","gauss15","gauss_seidel_linsolve","gaussian_window","gaussjord","gaussquad","gbasis","gbasis_max_pairs","gbasis_simult_primes","gcd","gcdex","genpoly","geometric","geometric_cdf","geometric_icdf","getDenom","getKey","getNum","getType","get_edge_attribute","get_edge_weight","get_graph_attribute","get_vertex_attribute","girth","gl_ortho","gl_quaternion","gl_rotation","gl_showaxes","gl_shownames","gl_texture","gl_x","gl_x_axis_color","gl_x_axis_name","gl_x_axis_unit","gl_xtick","gl_y","gl_y_axis_color","gl_y_axis_name","gl_y_axis_unit","gl_ytick","gl_z","gl_z_axis_color","gl_z_axis_name","gl_z_axis_unit","gl_ztick","gnuplot","goto","grad","gramschmidt","graph","graph2tex","graph3d2tex","graph_automorphisms","graph_charpoly","graph_complement","graph_diameter","graph_equal","graph_join","graph_power","graph_rank","graph_spectrum","graph_union","graph_vertices","graphe","graphe3d","graphe_suite","greduce","greedy_color","green","grid_graph","grid_paper","groupermu","hadamard","half_cone","half_line","halftan","halftan_hyp2exp","halt","hamdist","hamming_window","hann_poisson_window","hann_window","harmonic_conjugate","harmonic_division","has","has_arc","has_edge","hasard","head","heading","heapify","heappop","heappush","hermite","hessenberg","hessian","heugcd","hexagon","hidden_name","highlight_edges","highlight_subgraph","highlight_trail","highlight_vertex","highpass","hilbert","histogram","hold","homothety","horner","hybrid_solver","hybridj_solver","hybrids_solver","hybridsj_solver","hyp2exp","hyperbola","hypercube_graph","i","iPart","i[]","iabcuv","ibasis","ibpdv","ibpu","icdf","ichinrem","ichrem","icomp","icontent","icosahedron","id","identifier","identity","idivis","idn","iegcd","if","ifactor","ifactors","ifft","ifte","igamma","igcd","igcdex","ihermite","ilaplace","im","imag","image","implicitdiff","implicitplot","import_graph","in","inString","in_ideal","incidence_matrix","incident_edges","incircle","increasing_power","independence_number","indets","index","induced_subgraph","inequationplot","inf","infinity","infnorm","input","inputform","insert","insmod","int","intDiv","integer","integrate","integrer","inter","interactive_odeplot","interactive_plotode","interp","intersect","interval","interval2center","interval_graph","inv","inverse","inversion","invisible_point","invlaplace","invztrans","iquo","iquorem","iratrecon","irem","isPrime","is_acyclic","is_arborescence","is_biconnected","is_bipartite","is_clique","is_collinear","is_concyclic","is_conjugate","is_connected","is_coplanar","is_cospheric","is_cut_set","is_cycle","is_directed","is_element","is_equilateral","is_eulerian","is_forest","is_graphic_sequence","is_hamiltonian","is_harmonic","is_harmonic_circle_bundle","is_harmonic_line_bundle","is_included","is_inside","is_integer_graph","is_isomorphic","is_isosceles","is_network","is_orthogonal","is_parallel","is_parallelogram","is_permu","is_perpendicular","is_planar","is_prime","is_pseudoprime","is_rectangle","is_regular","is_rhombus","is_square","is_strongly_connected","is_strongly_regular","is_tournament","is_tree","is_triconnected","is_two_edge_connected","is_vertex_colorable","is_weighted","ismith","isobarycenter","isom","isomorphic_copy","isopolygon","isosceles_triangle","isprime","ithprime","jacobi_equation","jacobi_linsolve","jacobi_symbol","jordan","jusqu_a","jusqua","jusque","kde","keep_algext","keep_pivot","ker","kernel","kernel_density","kill","kneser_graph","kolmogorovd","kolmogorovt","kovacicsols","l1norm","l2norm","label","labels","lagrange","laguerre","laplace","laplacian","laplacian_matrix","latex","lcf_graph","lcm","lcoeff","ldegree","left","left_rectangle","legend","legendre","legendre_symbol","len","length","leve_crayon","lgcd","lhs","ligne_chapeau_carre","ligne_chapeau_plat","ligne_chapeau_rond","ligne_polygonale","ligne_polygonale_pointee","ligne_tiret","ligne_tiret_point","ligne_tiret_pointpoint","ligne_trait_plein","limit","limite","lin","line","line_graph","line_inter","line_paper","line_segments","line_width_1","line_width_2","line_width_3","line_width_4","line_width_5","line_width_6","line_width_7","linear_interpolate","linear_regression","linear_regression_plot","lineariser","lineariser_trigo","linfnorm","linsolve","linspace","lis","lis_phrase","list","list2exp","list2mat","list_edge_attributes","list_graph_attributes","list_vertex_attributes","listplot","lll","ln","lname","lncollect","lnexpand","local","locus","log","log10","logarithmic_regression","logarithmic_regression_plot","logb","logistic_regression","logistic_regression_plot","lower","lowest_common_ancestor","lowpass","lp_assume","lp_bestprojection","lp_binary","lp_binaryvariables","lp_breadthfirst","lp_depthfirst","lp_depthlimit","lp_firstfractional","lp_gaptolerance","lp_hybrid","lp_initialpoint","lp_integer","lp_integertolerance","lp_integervariables","lp_interiorpoint","lp_iterationlimit","lp_lastfractional","lp_maxcuts","lp_maximize","lp_method","lp_mostfractional","lp_nodelimit","lp_nodeselect","lp_nonnegative","lp_nonnegint","lp_pseudocost","lp_simplex","lp_timelimit","lp_variables","lp_varselect","lp_verbose","lpsolve","lsmod","lsq","lu","lvar","mRow","mRowAdd","magenta","make_directed","make_weighted","makelist","makemat","makesuite","makevector","map","maple2mupad","maple2xcas","maple_ifactors","maple_mode","markov","mat2list","mathml","matpow","matrix","matrix_norm","max","maxflow","maximal_independent_set","maximize","maximum_clique","maximum_degree","maximum_independent_set","maximum_matching","maxnorm","mean","median","median_line","member","mgf","mid","middle_point","midpoint","min","minimal_edge_coloring","minimal_spanning_tree","minimal_vertex_coloring","minimax","minimize","minimum_degree","minus","mkisom","mksa","mod","modgcd","mods","montre_tortue","moustache","moyal","moyenne","mul","mult_c_conjugate","mult_conjugate","multinomial","multiplier_conjugue","multiplier_conjugue_complexe","multiply","mupad2maple","mupad2xcas","mycielski","nCr","nDeriv","nInt","nPr","nSolve","ncols","negbinomial","negbinomial_cdf","negbinomial_icdf","neighbors","network_transitivity","newList","newMat","newton","newton_solver","newtonj_solver","nextperm","nextprime","nlpsolve","nodisp","nom_cache","non","non_recursive_normal","nop","nops","norm","normal","normal_cdf","normal_icdf","normald","normald_cdf","normald_icdf","normalize","normalt","normalvariate","not","nprimes","nrows","nuage_points","nullspace","number_of_edges","number_of_spanning_trees","number_of_triangles","number_of_vertices","numer","octahedron","od","odd","odd_girth","odd_graph","odeplot","odesolve","of","op","open","open_polygon","option","or","ord","order","order_size","ordinate","orthocenter","orthogonal","osculating_circle","otherwise","ou","output","p1oc2","p1op2","pa2b2","pade","parabola","parallel","parallelepiped","parallelogram","parameq","parameter","paramplot","parfrac","pari","part","partfrac","parzen_window","pas","pas_de_cote","path_graph","pcar","pcar_hessenberg","pcoef","pcoeff","pencolor","pendown","penup","perimeter","perimeterat","perimeteratraw","periodic","perm","perminv","permu2cycles","permu2mat","permuorder","permute_vertices","perpen_bisector","perpendicular","petersen_graph","peval","pi","piecewise","pivot","pixoff","pixon","planar","plane","plane_dual","playsnd","plex","plot","plot3d","plotarea","plotcdf","plotcontour","plotdensity","plotfield","plotfunc","plotimplicit","plotinequation","plotlist","plotode","plotparam","plotpolar","plotproba","plotseq","plotspectrum","plotwav","plus_point","pmin","point","point2d","point3d","point_carre","point_croix","point_etoile","point_invisible","point_losange","point_milieu","point_plus","point_point","point_triangle","point_width_1","point_width_2","point_width_3","point_width_4","point_width_5","point_width_6","point_width_7","poisson","poisson_cdf","poisson_icdf","poisson_window","polar","polar_coordinates","polar_point","polarplot","pole","poly2symb","polyEval","polygon","polygone_rempli","polygonplot","polygonscatterplot","polyhedron","polynom","polynomial_regression","polynomial_regression_plot","position","poslbdLMQ","posubLMQ","potential","pour","pow","pow2exp","power_regression","power_regression_plot","powermod","powerpc","powexpand","powmod","prepend","preval","prevperm","prevprime","primpart","print","printf","prism","prism_graph","proc","product","program","projection","proot","propFrac","propfrac","psrgcd","ptayl","purge","pwd","pyramid","python_compat","q2a","qr","quadrant1","quadrant2","quadrant3","quadrant4","quadric","quadrilateral","quantile","quartile1","quartile3","quartiles","quest","quo","quorem","quote","r2e","radical_axis","radius","ramene","rand","randMat","randNorm","randPoly","randbetad","randbinomial","randchisquare","randexp","randfisher","randgammad","randgeometric","randint","randmarkov","randmatrix","randmultinomial","randnorm","random","random_bipartite_graph","random_digraph","random_graph","random_network","random_planar_graph","random_regular_graph","random_sequence_graph","random_tournament","random_tree","random_variable","randperm","randpoisson","randpoly","randseed","randstudent","randvar","randvector","randweibulld","range","rank","ranm","ranv","rassembler_trigo","rat_jordan","rational","rationalroot","ratnormal","rcl","rdiv","re","read","readrgb","readwav","real","realroot","reciprocation","rectangle","rectangle_droit","rectangle_gauche","rectangle_plein","rectangular_coordinates","recule","red","redim","reduced_conic","reduced_quadric","ref","reflection","regroup","relabel_vertices","reliability_polynomial","rem","remain","remove","reorder","repeat","repete","repeter","replace","resample","residue","resoudre","resoudre_dans_C","resoudre_systeme_lineaire","restart","resultant","return","reverse","reverse_graph","reverse_rsolve","revert","revlex","revlist","rgb","rhombus","rhombus_point","rhs","riemann_window","right","right_rectangle","right_triangle","risch","rm_a_z","rm_all_vars","rmbreakpoint","rmmod","rmwatch","romberg","rombergm","rombergt","rond","root","rootof","roots","rotate","rotation","round","row","rowAdd","rowDim","rowNorm","rowSwap","rowdim","rownorm","rowspace","rowswap","rref","rsolve","same","sample","samplerate","sans_factoriser","saute","sauve","save_history","scalarProduct","scalar_product","scale","scaleadd","scatterplot","schur","sec","secant_solver","segment","seidel_spectrum","seidel_switch","select","semi_augment","seq","seqplot","seqsolve","sequence_graph","series","set[]","set_edge_attribute","set_edge_weight","set_graph_attribute","set_pixel","set_vertex_attribute","set_vertex_positions","shift","shift_phase","shortest_path","show_pixels","shuffle","si","sierpinski_graph","sign","signature","signe","similarity","simp2","simplex_reduce","simplifier","simplify","simpson","simult","sin","sin2costan","sincos","single_inter","sinh","sinon","size","sizes","slope","slopeat","slopeatraw","smith","smith","smod","snedecor","snedecor_cdf","snedecor_icdf","snedecord","snedecord_cdf","snedecord_icdf","solid_line","solve","somme","sommet","sort","sorta","sortd","sorted","soundsec","spanning_tree","sphere","spline","split","spring","sq","sqrfree","sqrt","square","square_point","srand","sst","sst_in","st_ordering","stack","star_graph","star_point","start","stdDev","stddev","stddevp","steffenson_solver","step","stereo2mono","sto","str","string","string","strongly_connected_components","student","student_cdf","student_icdf","studentd","studentt","sturm","sturmab","sturmseq","style","subMat","subdivide_edges","subgraph","subs","subsop","subst","substituer","subtype","sum","sum_riemann","suppress","surd","svd","swapcol","swaprow","switch","switch_axes","sylvester","symb2poly","symbol","syst2mat","tCollect","tExpand","table","tablefunc","tableseq","tabvar","tail","tan","tan2cossin2","tan2sincos","tan2sincos2","tangent","tangente","tanh","tantque","taux_accroissement","taylor","tchebyshev1","tchebyshev2","tcoeff","tcollect","tdeg","tensor_product","test","tetrahedron","texpand","textinput","then","thickness","thiele","threshold","throw","time","title","titre","tlin","to","tonnetz","topologic_sort","topological_sort","torus_grid_graph","tourne_droite","tourne_gauche","tpsolve","trace","trail","trail2edges","trames","tran","transitive_closure","translation","transpose","trapeze","trapezoid","traveling_salesman","tree","tree_height","triangle","triangle_paper","triangle_plein","triangle_point","triangle_window","trig2exp","trigcos","trigexpand","triginterp","trigsimplify","trigsin","trigtan","trn","true","trunc","truncate","try","tsimplify","tuer","tukey_window","tutte_polynomial","two_edge_connected_components","type","ufactor","ugamma","unapply","unarchive","underlying_graph","unfactored","uniform","uniform_cdf","uniform_icdf","uniformd","uniformd_cdf","uniformd_icdf","union","unitV","unquote","until","upper","user_operator","usimplify","valuation","vandermonde","var","variables_are_files","variance","vector","vector","vers","version","vertex_connectivity","vertex_degree","vertex_distance","vertex_in_degree","vertex_out_degree","vertices","vertices_abc","vertices_abca","vpotential","watch","web_graph","weibull","weibull_cdf","weibull_icdf","weibulld","weibulld_cdf","weibulld_icdf","weibullvariate","weight_matrix","weighted","weights","welch_window","wheel_graph","when","while","white","widget_size","wilcoxonp","wilcoxons","wilcoxont","with_sqrt","write","writergb","writewav","wz_certificate","xcas_mode","xor","xyztrange","yellow","zeros","zip","ztrans","{}","|","||"],
  dicho_find: function (tableau, s) {
    var l = tableau.length, debut = 0, fin = l, milieu;
    if (l == 0) return false;
    if (s < tableau[0] || s > tableau[l - 1]) return false;
    // s>=tableau[debut] and s<=tableau[fin-1]
    for (; debut < fin - 1;) {
      milieu = Math.floor((debut + fin) / 2);
      // console.log(debut,fin,milieu,tableau[milieu])
      if (s >= tableau[milieu]) debut = milieu; else fin = milieu;
    }
    // console.log(s,tableau[debut]);
    if (s == tableau[debut]) return true;
    return false;
  },
  unique_completion: function (tableau, s) {
    var l = tableau.length, debut = 0, fin = l, milieu;
    if (l == 0 || s > tableau[l - 1]) return "";
    if (s < tableau[0]) return s == tableau[0].substr(s.length);
    // s>=tableau[debut] and s<=tableau[fin-1]
    for (; debut < fin - 1;) {
      milieu = Math.floor((debut + fin) / 2);
      // console.log(debut,fin,milieu,tableau[milieu])
      if (s >= tableau[milieu]) debut = milieu; else fin = milieu;
    }
    // console.log(tableau[debut+1],tableau[debut+1].length);
    // console.log(s,tableau[debut],tableau[debut+1],tableau[debut+2]);
    // s>=tableau[debut]
    if (s == tableau[debut]) {
      if (debut + 1 < l && s == tableau[debut + 1].substr(0, s.length)) return "";
      return s;
    }
    // now s>tableau[debut] and s is supposed to be shorter, hence s!=begin of tableau[debut]
    if (debut + 1 < l && s != tableau[debut + 1].substr(0, s.length)) return "";
    if (debut + 2 < l && s == tableau[debut + 2].substr(0, s.length)) return "";
    return tableau[debut + 1];
  },
  completion: function (cm) {
    var s, k;
    if (cm.type == 'textarea') {
      k = cm.selectionStart;
      s = cm.value;
    }
    else {
      var pos = cm.getCursor();
      k = pos.ch;
      s = cm.getLine(pos.line);
    }
    var kstart = k;
    // skip at cursor
    for (; k > 0; k--) {
      var c = s.charCodeAt(k);
      if (UI.is_alphan(c))
        break;
    }
    var kend = k;
    for (; k >= 0; k--) {
      var c = s.charCodeAt(k);
      if (!UI.is_alphan(c))
        break;
    }
    for (; k < kend; k++) {
      var c = s.charCodeAt(k + 1);
      if (c > 64) break;
    }
    //Module.print(s); Module.print(k); Module.print(kend);
    if (s.length < 2) {
      UI.insert(UI.focused, '?');
      return;
    }
    s = s.substr(k + 1, kend - k);
    var sc = UI.unique_completion(UI.xcascmd, s);
    //Module.print(s); Module.print(sc);
    if (cm.type == 'textarea') {
      cm.selectionStart = k + 1;
      cm.selectionEnd = kstart;
      if (sc != "") {
        UI.insert(cm, sc);
        s = sc;
        cm.selectionStart = k + 1;
        cm.selectionEnd = k + 1 + sc.length;
      }
    } else {
      //console.log(sc.length);
      if (sc != "") {
        //console.log(s,k);
        cm.setSelection({line: pos.line, ch: k + 1}, {line: pos.line, ch: pos.ch});
        UI.insert(cm, sc);
        //cm.setSelection({line:pos.line,ch:k+1},{line:pos.line,ch:k+1+sc.length});
        cm.setSelection({line: pos.line, ch: k + 1 + sc.length}, {line: pos.line, ch: k + 1 + sc.length});
        s = sc;
      }
      else {
        //console.log(s,k);
        cm.setCursor({line: pos.line, ch: k + 1 + s.length});
        cm.showHint();
      }
    }
    UI.addhelp('?', s);
  },
  isie: false,
  scrollatend: function (field) {
    if (!UI.isie)
      field.scrollTop = field.scrollHeight;
  },
  listvars: function (n) { // n=2 (list) or 3 (list+values)
    var s = UI.caseval_noautosimp('VARS(' + n + ')');
    s = s.replace(/\"\"/g, '\\\"');
    s = s.replace(/\n/g, '\\n');
    //console.log(s);
    var v = eval(s); // s should be an array of strings
    var f = $id('listvars');
    if (!Array.isArray(v)) {
      f.innerHTML = '<br>' + s;
      return;
    }
    var l = v.length, i, j, cs;
    s = "";
    for (i = 0; i < l; i += 2) {
      var name = v[i];
      var value = v[i + 1], nl=UI.count_newline(value)+1;
      if (nl>5) nl=5;
      s += '<button class="bouton" onclick="UI.insert(UI.focused,\'' + name + '\')">' + name + '</button>';
      //console.log(value);
      if (value.length >= 6 && value.substr(0, 6) == "\n<math")
        s += value;
      else
        s += '<textarea readonly cols=30 ' + 'style="height:' + (20 + 16 * nl) + 'px; font-size:large">' + value + "</textarea>";
    }
    //console.log(s);
    f.innerHTML = '<br>' + s;
  },
  addhelp: function (prefixe, text) {
    $id('helptxt').value = text;
    var input = prefixe + text;
    var out = UI.eval(input, input);
    var add = out;
    var helpoutput = $id('helpoutput');
    helpoutput.innerHTML += add;
    UI.scrollatend(helpoutput.parentNode); // focus at end
    if (UI.focusaftereval) UI.focused.focus();
  },
  clean: function (text, quote) {
    var cmd = text;
    if (quote) cmd = cmd.replace(/\'/g, '\\\'');
    cmd = cmd.replace(/>/g, '&gt;');
    cmd = cmd.replace(/</g, '&lt;');
    if (quote) cmd = cmd.replace(/\"\"/g, '&quote;');
    return cmd;
  },
  renderhelp: function (text, localized_cmd) {
    var s = text.substr(1, text.length - 2);
    var pos0 = s.search("</b>");
    var found = (s.substr(pos0 + 7, 20) != "Best match has score");
    var lh = s.substr(3, pos0 - 3);
    var unlocalized_cmd = lh;
    //console.log(localized_cmd,unlocalized_cmd);
    if (found) {
      var tmp = eval('longhelp.' + lh);
      if (tmp == undefined) lh = '';
      else {
        var lang = tmp.substr(7, 2);
        var prefix = UI.docprefix;
        prefix = prefix.substr(0, prefix.length - 13) + lang + '/cascmd_' + lang + '/';
        //console.log(prefix,tmp);
        lh = ' (<a href="' + prefix + tmp + '" target="_blank">' + (UI.langue == -1 ? '+ de d&eacute;tails' : 'more details') + '</a>)';
      }
    }
    //Module.print(lh);
    var sorig = s;
    pos1 = s.search("<br>");
    if (pos1 < 0) return sorig;
    var explication = s.substr(0, pos1);
    s = s.substr(pos1 + 4, s.length - pos1 - 4);
    pos1 = s.search("<br>");
    if (pos1 < 0) return sorig;
    var syntaxe = s.substr(0, pos1);
    s = s.substr(pos1 + 4, s.length - pos1 - 4);
    pos1 = s.search("<br>");
    if (pos1 < 0) return sorig;
    var voiraussi = s.substr(0, pos1);
    var examples = s.substr(pos1 + 4, s.length - pos1 - 4);
    if (found)
      s = explication + lh + '<br><tt>' + syntaxe + '</tt><br>' + (UI.langue == -1 ? 'Voir aussi: ' : 'See also ');
    else
      s = explication.substr(0, pos0);
    while (true) {
      pos1 = voiraussi.search(',');
      if (pos1 < 0) break;
      var cmd = UI.clean(voiraussi.substr(0, pos1), false);
      if (found)
        s += '<button class="bouton" onmousedown="event.preventDefault()" onclick="UI.addhelp(\'?\',\'' + cmd + '\')">' + cmd + '</button>';
      else
        s += '<button class="bouton" onmousedown="event.preventDefault()" onclick="var help=$id(\'helptxt\');help.value=\'' + cmd + '\'; UI.insert(UI.focused!=help?UI.focused:cmentree,\'' + UI.clean(cmd, true) + '(\')">' + cmd + '</button>';
      voiraussi = voiraussi.substr(pos1 + 1, voiraussi.length - pos1 - 1);
    }
    if (found)
      s += '<button class="bouton" onmousedown="event.preventDefault()" onclick="UI.addhelp(\'?\',\'' + voiraussi + '\')">' + voiraussi + '</button>';
    else
      s += '<button class="bouton" onmousedown="event.preventDefault()" onclick="UI.insert(UI.focused,\'' + voiraussi + '(\')">' + voiraussi + '</button>';
    pos1 = examples.search(';');
    if (pos1 >= 0) {
      s += '<br>' + (UI.langue == -1 ? 'Exemples: ' : 'Examples ');
      while (true) {
        pos1 = examples.search(';');
        if (pos1 < 0) break;
        var cmd = examples.substr(0, pos1);
        //console.log('avant',cmd);
        cmd = cmd.replace(unlocalized_cmd, localized_cmd);
        //console.log('apres',cmd);
        cmd = UI.clean(cmd, false);
        s += '<button class="bouton" onmousedown="event.preventDefault()" onclick="UI.insert(UI.focused,\'' + UI.clean(cmd, true) + '\')">' + cmd + '</button>';
        examples = examples.substr(pos1 + 1, examples.length - pos1 - 1);
      }
    }
    s += '<button class="bouton" onmousedown="event.preventDefault()" onclick="UI.insert(UI.focused,\'' + UI.clean(examples, true) + '\')">' + examples + '</button>';
    //console.log(s);
    return s;
  },
  getsel: function (field) {
    var startPos = field.selectionStart;
    var endPos = field.selectionEnd;
    var selectedText = field.value.substring(startPos, endPos);
    return selectedText;
  },
  move_caret_or_focus: function (field, n) {
    UI.moveCaret(field, n);
    return;
    if (UI.detectmob()) {
      UI.moveCaret(field, n);
      return;
    }
    while (n >= 1) {
      UI.move_focus(field, 1);
      n--;
    }
    while (n <= -1) {
      UI.move_focus(field, -1);
      n++;
    }
  },
  set_focused: function (field) {
    UI.focused = field;
  },
  selectionne: function () {
    UI.focused.focus();
    if (UI.focused.type != 'textarea') {
      if (UI.focused.execCommand)
        UI.focused.execCommand('selectAll');
    }
    else {
      UI.focused.select();
      UI.selection = UI.focused.value;
    }
  },
  set_focus: function (s) {
    var tmp = $id(s);
    UI.focused = tmp;
    UI.selectionne(); // if (!UI.is_touch_device()) tmp.focus();
  },
  adjust_focus: function (field) {
    var p1 = field.parentNode;
    if (p1) {
      p1 = p1.nextSibling;
      if (p1) {
        p1 = p1.firstChild;
        if (p1) {
          p1 = p1.nextSibling;
          if (p1 && p1.title == 'Editer ce commentaire') {
            p1.click();
            return;
          }
        }
      }
    }
    var p2 = field.nextSibling;
    if (p2.CodeMirror) {
      p2.CodeMirror.toTextArea();
    }
    field.focus();
  },
  move_focus_end: function (par) {
    if (par == null || par.firstChild == null) return;
    UI.switch_buttons(par.firstChild, true)
    par = par.firstChild.firstChild.nextSibling.nextSibling.nextSibling.firstChild;
    if (par.nextSibling == null) {
      par = par.parentNode.previousSibling.firstChild;
      UI.adjust_focus(par);
      return;
    }
    if (par.nextSibling.title == "Editer ce commentaire") {
      par.nextSibling.click();
      return;
    }
    //console.log(par.innerHTML);
    par.style.display = 'none';
    par = par.nextSibling;
    par.style.display = 'inherit';
    par.select();
    UI.set_focused(par);
    UI.adjust_focus(par);
    return;
  },
  move_focus_end2: function (par) {
    par = par.parentNode.parentNode.parentNode;
    if (!par.nextSibling) par = par.parentNode; // inside a comment 1 more level
    par = par.nextSibling;
    if (par == null || par.firstChild == null) {
      UI.set_focused(cmentree);
      cmentree.focus();
    }
    else {
      // console.log(par);
      UI.switch_buttons(par.firstChild, true)
      par = par.firstChild.firstChild.nextSibling.nextSibling.firstChild;
      UI.adjust_focus(par);
    }
  },
  move_focus: function (field, n) {
    // focus on next or previous history level
    if (n > 0) {
      if (field == cmentree) return;
      if (field.type != "textarea" && field.type != "text")
        field = field.getTextArea();
      if (field.previousSibling == null) {
        var par = field.parentNode;
        UI.switch_buttons(par.parentNode, true)
        par = par.nextSibling;
        par = par.firstChild;
        if (par.nextSibling == null || !par.style || !par.nextSibling.style) {
          UI.move_focus_end2(par);
          return;
        }
        par.style.display = 'none';
        par = par.nextSibling;
        par.style.display = 'inherit';
        par.select();
        UI.set_focused(par);
        UI.adjust_focus(par);
        return;
      }
      var bidon = field.previousSibling;
      if (bidon.style.display != 'none') return;
      bidon.style.display = 'block';
      field.style.display = 'none';
      UI.move_focus_end2(field);
      return;
    } // end if n>0
    if (field == cmentree) {
      var par = $id('mathoutput').lastChild;
      if (par == null) return;
      UI.move_focus_end(par);
      return;
    }
    if (field.type != "textarea" && field.type != "text")
      field = field.getTextArea();
    if (field.previousSibling == null) {
      var par = field.parentNode.parentNode.parentNode;
      if (!par.previousSibling) par = par.parentNode; // inside a comment 1 more level
      par = par.previousSibling;
      UI.move_focus_end(par);
      return;
    }
    var bidon = field.previousSibling;
    if (bidon.style.display != 'none') return;
    bidon.style.display = 'block';
    field.style.display = 'none';
    var par = field.parentNode.previousSibling.firstChild;
    UI.adjust_focus(par);
    UI.switch_buttons(par.parentNode.parentNode, true)
  },
  selline: 0,
  selch: 0,
  setselbeg: function (field) {
    if (field.type != "textarea" && field.type != "text") {
      var pos = field.getCursor(); // save position
      UI.selline = pos.line;
      UI.selch = pos.ch;
      //console.log(UI.selline,UI.selch);
      return;
    }
    UI.selch = field.selectionStart;
  },
  setselend: function (field) {
    if (field.type != "textarea" && field.type != "text") {
      var startpos = field.getCursor(); // current position
      field.setSelection({line: UI.selline, ch: UI.selch}, startpos);
      field.refresh();
      UI.selection = field.getSelection();
      return;
    }
    var pos1 = field.selectionStart;
    var pos2 = UI.selch;
    if (pos2 > field.value.length) pos2 = field.value.length;
    if (pos1 > pos2) {
      var tmp = pos1;
      pos1 = pos2;
      pos2 = tmp;
    }
    field.setSelectionRange(pos1, pos2);
    UI.selection = field.value.substr(pos1, pos2 - pos1);
  },
  indent_or_complete: function (field) {
    if (field.type != 'textarea') {
      if (field.lineCount() == 1)
        UI.completion(field);
      else
        field.execCommand('indentAuto');
    }
  },
  indentline: function (field) {
    if (field.type != 'textarea') {
      field.execCommand('indentAuto');
    }
  },
  moveCaret: function (field, charCount) {
    if (field.type != "textarea" && field.type != "text") {
      var pos = field.getCursor();
      pos.ch = pos.ch + charCount;
      field.setCursor(pos);
      field.refresh();
      // UI.show_curseur();
      return;
    }
    var pos = field.selectionStart;
    pos = pos + charCount;
    if (pos < 0) pos = 0;
    if (pos > field.value.length) pos = field.value.length;
    field.setSelectionRange(pos, pos);
  },
  show_curseur: function () {
    document.getElementsByClassName("CodeMirror-cursors")[0].style.visibility = "visible";
    var cursors = document.getElementsByClassName("CodeMirror-cursor");
    for (var i = 0; i < cursors.length; i++) {
      cursors[i].style.visibility = "visible";
    }
  },
  moveCaretUpDown: function (field, Count) {
    if (field.type != "textarea" && field.type != "text") {
      var pos = field.getCursor();
      pos.line = pos.line + Count;
      field.setCursor(pos);
      field.refresh();
      //UI.show_curseur();
      return;
    }
    if (Count < -1) {
      var i;
      for (i = 0; i > Count; i--)
        UI.moveCaretUpDown(field, -1);
      return;
    }
    if (Count > 1) {
      var i;
      for (i = 0; i < Count; i++)
        UI.moveCaretUpDown(field, 1);
      return;
    }
    var pos = field.selectionStart;
    var s = field.value;
    var cur = pos, shift = pos + 1, pos1;
    cur--;
    if (cur >= s.length) cur--;
    for (; cur >= 0; cur--) {
      if (s.charCodeAt(cur) == 10) {
        shift = pos - cur;
        break;
      }
    }
    if (Count == -1) {
      if (cur < 0) return;
      pos1 = cur;
      cur--;
      for (; cur >= 0; cur--) {
        if (s.charCodeAt(cur) == 10) break;
      }
      //console.log(cur,shift);
      pos = cur + shift;
      if (pos > pos1) pos = pos1;
    }
    if (Count == 1) {
      cur = pos;
      for (; cur < s.length; cur++) {
        if (s.charCodeAt(cur) == 10) break;
      }
      pos = cur + shift;
      if (pos >= s.length) return;
      pos1 = pos;
      for (; pos1 > cur; pos1--) {
        if (s.charCodeAt(pos1) == 10) pos = pos1;
      }
    }
    if (pos < 0) pos = 0;
    if (pos > field.value.length) pos = field.value.length;
    field.setSelectionRange(pos, pos);
  },
  erase: function (field) {
    var par = field.parentNode;
    par = par.parentNode;
    //par.style.display='none';
    par.style.visibility = 'hidden';
    if (UI.focusaftereval) UI.focused.focus();
    par = par.parentNode;
    var list = par.parentNode;
    if (list.id == 'helpoutput')
      list.removeChild(par);
  },
  erase_all_warn: 1,
  erase_all: function (field) {
    var cur = field.firstChild;
    while (cur) {
      //cur.firstChild.style.display='none';
      cur.firstChild.style.visibility = 'hidden';
      cur = cur.nextSibling;
    }
    if (UI.erase_all_warn == 1) {
      if (UI.langue == -1)
        alert('Pour annuler, cliquez sur Restaure. Pour confirmer, cliquez sur Vide.');
      else
        alert('Click on Restore to undo. Or confirm by clicking on Empty.');
      UI.erase_all_warn = 0;
    }
    if (UI.focusaftereval) cmentree.focus();
  },
  restoretrash: function () {
    var hist = $id('mathoutput');
    var cur = hist.firstChild;
    while (cur) {
      //if (cur.style.display=='none') cur.style.display='block';
      if (cur.firstChild.style.visibility == 'hidden') cur.firstChild.style.visibility = 'visible';
      cur = cur.nextSibling;
    }
  },
  emptytrash: function () {
    var hist = $id('mathoutput');
    var cur = hist.firstChild;
    while (cur) {
      var nxt = cur.nextSibling;
      //if (cur.firstChild.style.display=='none') hist.removeChild(cur);
      if (cur.firstChild.style.visibility == 'hidden') hist.removeChild(cur);
      cur = nxt;
    }
    UI.link(0);
  },
  updatelevel: function (field) {
    UI.set_editline(field, false);
    UI.set_config_width();
    var pos = field.selectionStart;
    field.innerHTML = field.value;
    if (pos >= 0 && pos < field.value.length)
      field.setSelectionRange(pos, pos);
  },
  moveup: function (field) {
    var par = field.parentNode;
    par = par.parentNode;
    par = par.parentNode;
    var prev = par.previousSibling;
    var list = par.parentNode;
    list.removeChild(par);
    list.insertBefore(par, prev);
    par.scrollIntoView(false);
    if (UI.focusaftereval) UI.focused.focus();
    UI.link(0);
  },
  movedown: function (field) {
    var par = field.parentNode;
    par = par.parentNode;
    par = par.parentNode;
    var nxt = par.nextSibling;
    nxt = nxt.nextSibling;
    var list = par.parentNode;
    list.removeChild(par);
    list.insertBefore(par, nxt);
    par.scrollIntoView(true);
    UI.link(0);
  },
  backspace: function (field) {
    //if (UI.focusaftereval) field.focus();
    if (field.type != "textarea" && field.type != "text") {
      var start = field.getCursor('from');
      var end = field.getCursor('to');
      if (end.line != start.line || end.ch != start.ch)
        field.replaceSelection('');
      else {
        var c = start.ch;
        var l = start.line;
        if (start.ch == 0 && start.line == 0) return;
        if (c > 0) {
          var s = field.getRange({line: l, ch: 0}, end), i;
          for (i = 0; i < s.length; i++) {
            if (s.charAt(i) != ' ') break;
          }
          //console.log(i,s.length,c);
          if (i == s.length && c >= 2) {
            var l1 = l - 1;
            for (; l1 >= 0; --l1) {
              s = field.getLine(l1);
              for (i = 0; i < s.length && i < c; i++) {
                if (s[i] != ' ') break;
              }
              if (i != s.length && i < c) break;
            }
            if (l1 >= 0) c = i; else c -= 2;
          }
          else c--;
          field.replaceRange('', {line: l, ch: c}, end);
        }
        else {
          l--;
          var s = field.getRange({line: l, ch: c}, end);
          field.replaceRange('', {line: l, ch: s.length - 1}, end);
        }
      }
      var t = field.getTextArea();
      t.value = field.getValue();
    } else {
      var pos = field.selectionStart;
      var pos2 = field.selectionEnd;
      var s = field.value;
      if (pos < pos2) {
        field.value = s.substring(0, pos) + s.substring(pos2, s.length);
        if (pos < 0) pos = 0;
        if (pos > field.value.length) pos = field.value.length;
        field.setSelectionRange(pos, pos);
        UI.resizetextarea(field);
        return;
      }
      if (pos > 0) {
        field.value = s.substring(0, pos - 1) + s.substring(pos, s.length);
        pos--;
        if (pos < 0) pos = 0;
        if (pos > field.value.length) pos = field.value.length;
        field.setSelectionRange(pos, pos);
        UI.resizetextarea(field);
      }
    }
  },
  insert: function (field, value) {
    var myValue = value.replace(/&quote;/g, '\"');
    //console.log('2',field);
    if (UI.focusaftereval) field.focus();
    if (field.type != "textarea" && field.type != "text") {
      if (field.type == undefined && field.firstChild != undefined) {
        // console.log(field.innerHTML);
        UI.insert(field.firstChild, value);
        return;
      }
      var start = field.getCursor('from');
      var end = field.getCursor('to');
      if (end.line != start.line || end.ch != start.ch || myValue.length < 3)
        field.replaceSelection(myValue);
      else {
        // detect the same command not selected just before
        var parpos = 0;
        for (; parpos < myValue.length; ++parpos) {
          if (myValue[parpos] == '(') break;
        }
        if (parpos < myValue.length) {
          var S = {line: start.line, ch: start.ch};
          S.ch -= (parpos + 1);
          var deb = S.ch == -1;
          if (deb) S.ch = 0;
          if (S.ch >= 0) {
            var avant = field.getRange(S, end);
            //console.log('avant avant',avant,S.ch,deb);
            var tst = avant.length - 1;
            if (!deb && avant[tst] != '(') {
              avant = avant.substr(1, avant.length - 1);
              //console.log('avant avant',avant);
              --tst;
            }
            else {
              if (avant[tst] == '(')
                ++parpos;
            }
            //console.log('avant apres',avant);
            for (; tst >= 0; --tst) {
              if (myValue[tst] != avant[tst]) break;
            }
            //console.log('avant',myValue);
            if (tst < 0)
              myValue = myValue.substr(parpos, myValue.length - parpos);
            //console.log('apres',myValue);
          }
        }
        field.replaceSelection(myValue);
      }
      field.execCommand("indentAuto");
      var t = field.getTextArea();
      t.value = field.getValue();
    }
    else {
      var pos = field.selectionStart;
      pos = pos + myValue.length;
      //IE support
      if (document.selection) {
        if (UI.focusaftereval) field.focus();
        var sel = document.selection.createRange();
        sel.text = myValue;
      }
      //MOZILLA and others
      else {
        var startPos = field.selectionStart;
        var endPos = field.selectionEnd;
        if (field.selectionStart || field.selectionStart == '0') {
          field.value = field.value.substring(0, startPos)
              + myValue
              + field.value.substring(endPos, field.value.length);
        } else {
          field.value += myValue;
        }
      }
      field.setSelectionRange(pos, pos);
      UI.resizetextarea(field);
    }
    // UI.show_curseur();
  },
  insertsemi: function (myField, value) {
    if (UI.python_mode)
      return UI.insert(myField, '\n' + value);
    UI.insert(myField, value + ';\n');
  },
  resizetextarea: function (field) {
    if (field.type != 'textarea') return;
    var s = field.value;
    var C = field.cols;
    //console.log(C);
    var N = 0, i, j = 0, n = s.length, c;
    for (i = 0; i < n; i++, j++) {
      c = s.charCodeAt(i);
      if (c == 10 || j == C) {
        N++;
        j = 0;
      }
    }
    field.rows = N + 1;
  },
  changefontsize: function (field, size) {
    field.getWrapperElement().style["font-size"] = size + "px";
    field.refresh();
  },
  color_list: ['black',
    'red',
    'green',
    'yellow',
    'blue',
    'magenta',
    'cyan',
    'white',
    'silver',
    'gray',
    'maroon',
    'purple',
    'fuchsia',
    'lime',
    'olive',
    'navy',
    'teal',
    'aqua',
    'antiquewhite',
    'aquamarine',
    'azure',
    'beige',
    'bisque',
    'blanchedalmond',
    'blueviolet',
    'brown',
    'burlywood',
    'cadetblue',
    'chartreuse',
    'chocolate',
    'coral',
    'cornflowerblue',
    'cornsilk',
    'crimson',
    'cyan',
    'darkblue',
    'darkcyan',
    'darkgoldenrod',
    'darkgray',
    'darkgreen',
    'darkgrey',
    'darkkhaki',
    'darkmagenta',
    'darkolivegreen',
    'darkorange',
    'darkorchid',
    'darkred',
    'darksalmon',
    'darkseagreen',
    'darkslateblue',
    'darkslategray',
    'darkslategrey',
    'darkturquoise',
    'darkviolet',
    'deeppink',
    'deepskyblue',
    'dimgray',
    'dimgrey',
    'dodgerblue',
    'firebrick',
    'floralwhite',
    'forestgreen',
    'gainsboro',
    'ghostwhite',
    'gold',
    'goldenrod',
    'greenyellow',
    'grey',
    'honeydew',
    'hotpink',
    'indianred',
    'indigo',
    'ivory',
    'khaki',
    'lavender',
    'lavenderblush',
    'lawngreen',
    'lemonchiffon',
    'lightblue',
    'lightcoral',
    'lightcyan',
    'lightgoldenrodyellow',
    'lightgray',
    'lightgreen',
    'lightgrey',
    'lightpink',
    'lightsalmon',
    'lightseagreen',
    'lightskyblue',
    'lightslategray',
    'lightslategrey',
    'lightsteelblue',
    'lightyellow',
    'limegreen',
    'linen',
    'mediumaquamarine',
    'mediumblue',
    'mediumorchid',
    'mediumpurple',
    'mediumseagreen',
    'mediumslateblue',
    'mediumspringgreen',
    'mediumturquoise',
    'mediumvioletred',
    'midnightblue',
    'mintcream',
    'mistyrose',
    'moccasin',
    'navajowhite',
    'oldlace',
    'olivedrab',
    'orangered',
    'orchid',
    'palegoldenrod',
    'palegreen',
    'paleturquoise',
    'palevioletred',
    'papayawhip',
    'peachpuff',
    'peru',
    'pink',
    'plum',
    'powderblue',
    'rosybrown',
    'royalblue',
    'saddlebrown',
    'salmon',
    'sandybrown',
    'seagreen',
    'seashell',
    'sienna',
    'skyblue',
    'slateblue',
    'slategray',
    'slategrey',
    'snow',
    'springgreen',
    'steelblue',
    'tan',
    'thistle',
    'tomato',
    'turquoise',
    'violet',
    'wheat',
    'whitesmoke',
    'yellowgreen'],
  arc_en_ciel: function (k) {
    var r, g, b;
    k += 21;
    k %= 126;
    if (k < 0)
      k += 126;
    if (k < 21) {
      r = 251;
      g = 0;
      b = 12 * k;
    }
    if (k >= 21 && k < 42) {
      r = 251 - (12 * (k - 21));
      g = 0;
      b = 251;
    }
    if (k >= 42 && k < 63) {
      r = 0;
      g = (k - 42) * 12;
      b = 251;
    }
    if (k >= 63 && k < 84) {
      r = 0;
      g = 251;
      b = 251 - (k - 63) * 12;
    }
    if (k >= 84 && k < 105) {
      r = (k - 84) * 12;
      g = 251;
      b = 0;
    }
    if (k >= 105 && k < 126) {
      r = 251;
      g = 251 - (k - 105) * 12;
      b = 0;
    }
    return 'rgb(' + r + ',' + g + ',' + b + ')';
  },
  turtle_color: function (c) {
    if (c >= 0x100) {
      if (c < 0x17e)
        return UI.arc_en_ciel(c);
      //console.log('rgb('+Math.floor(c/(256*256))+','+(Math.floor(c/256) % 256)+','+(c%256)+')');
      var r=8*((c>>11) & 0x1f);
      var g=4*((c>>5) & 0x3f);
      var b=8*(c & 0x1f);
      return 'rgb(' + r + ',' + g + ',' + b + ')';
      // return 'rgb(' + Math.floor(c / (256 * 256)) + ',' + (Math.floor(c / 256) % 256) + ',' + (c % 256) + ')';
    }
    return UI.color_list[c];
  },
  pixon_draw: function (id, s) {
    var v = eval(s);
    if (!Array.isArray(v)) return;
    //console.log(v[0], v.length);
    var canvas = $id(id);
    var l = v.length, w = 0, h = 0;
    if (l < 2) return;
    var scale = v[0];
    for (var k = 1; k < l; k++) {
      var cur = v[k];
      var x = cur[0], y = cur[1];
      if (cur.length==3 && typeof cur[2]!="number"){
	x+=100;
	y+=16;
      }
      if (cur.length==4) {
        var tmp = cur[3];
	if (typeof tmp=="number"){
          if (tmp > 0) y += tmp; else x -= tmp;
	} else {
	  x+=100;
	  y+=16;
	}
      }
      //console.log(cur,x,y);
      if (x > w) w = x;
      if (y > h) h = y;
    }
    w = (w + 1) * scale;
    h = (h + 1) * scale;
    canvas.width = w;
    canvas.height = h;
    //console.log(h,w);
    if (canvas.getContext) {
      var ctx = canvas.getContext('2d');
      for (var k = 1; k < l; k++) {
        var cur = v[k], cl;
        //console.log(cur);
        if (!Array.isArray(cur) || (cl = cur.length) < 2) continue;
        // cur[0]=x, cur[1]=y, cur[2]=color, cur[3]=w if +, h if -
        var x = cur[0] * scale;
        var y = cur[1] * scale;
	if (cl>2 && typeof cur[2]=="string"){
	  console.log(cur[2]);
	  ctx.font = '16px serif';
	  ctx.fillStyle = 'black';
	  ctx.fillText(cur[2],x,y+16,100);
	  continue;
	}
        ctx.fillStyle = (cl > 2) ? UI.turtle_color(cur[2]) : 'black';
        if (cl < 4) {
          ctx.fillRect(x, y, scale, scale);
          continue;
        }
	if (typeof cur[3]=="string"){
	  ctx.font = '16px serif';
	  ctx.fillText(cur[3],x,y+16,100);
	  continue;
	}
        var h = cur[3] * scale, w = scale;
        if (h < 0) {
          w = -h;
          h = scale;
        }
        ctx.fillRect(x, y, w, h);
      }
    }
  },
  turtle_dx: 0, // shift frame
  turtle_dy: 0,
  turtle_z: 1,  // zoom factor
  turtle_maillage: 1,
  turtle_draw: function (id, s) {
    if (s.length < 7) return;
    s = s.substr(5, s.length - 6);
    if (s.length > 7 && s.substr(s, 6) == "pixon(") {
      UI.pixon_draw(id, s.substr(6, s.length - 7));
      return;
    }
    if (s.length < 6 || s.substr(s, 5) != "logo(")
      return;
    s = s.substr(5, s.length - 6);
    //console.log(s);
    var v = eval(s);
    if (!Array.isArray(v)) return;
    //console.log(v[0]);
    var canvas = $id(id);
    var w = canvas.width, h = canvas.height;
    if (canvas.getContext) {
      var ctx = canvas.getContext('2d');
      var turtlezoom = UI.turtle_z, turtlex = UI.turtle_dx, turtley = UI.turtle_dy;
      // maillage
      if (UI.turtle_maillage & 3) {
        ctx.fillStyle = 'black';
        var xdecal = Math.floor(turtlex / 10.0) * 10;
        var ydecal = Math.floor(turtley / 10.0) * 10;
        if ((UI.turtle_maillage & 0x3) == 1) {
          for (var i = xdecal; i < w + xdecal; i += 10) {
            for (var j = ydecal; j < h + ydecal; j += 10) {
              var X = Math.floor((i - turtlex) * turtlezoom + .5);
              var Y = Math.floor((j - turtley) * turtlezoom + .5);
              // console.log(X,Y);
              ctx.fillRect(X, h - Y, 1, 1);
            }
          }
        } else {
          var dj = Math.sqrt(3.0) * 10, i0 = xdecal;
          for (var j = ydecal; j < h + ydecal; j += dj) {
            var J = Math.floor(h - (j - turtley) * turtlezoom);
            for (var i = i0; i < w + xdecal; i += 10) {
              ctx.fillRect(Math.floor((i - turtlex) * turtlezoom + .5), J, 1, 1);
            }
            i0 += dj;
            while (i0 >= 10)
              i0 -= 10;
          }
        }
      }
      var l = v.length, i;
      // montre la position et le cap (v[l-1])
      var prec = v[l - 1];
      ctx.font = '16px serif';
      ctx.fillStyle = 'yellow';
      ctx.fillRect(w - 40, 0, 40, 50);
      ctx.fillStyle = 'black';
      ctx.fillText('x:' + prec[0], w - 40, 15);
      ctx.fillText('y:' + prec[1], w - 40, 31);
      ctx.fillText('t:' + prec[2], w - 40, 49);
      // v[i]=[x(0),y(1),cap(2),status(3),r(4),chaine(5)],
      // couleur=status >> 11
      // longueur_tortue= (status>>3)&0xff
      // direct=status&4 (vrai si angle dans le sens trigo)
      // visible=status&2
      // crayon baisse=status&1
      // si r>0 arc/disque rayon=r & 0x1ff, theta1=(r >> 9) & 0x1ff, theta2=(r >> 18) & 0x1ff
      //        rempli=(r>>27)&0x1
      // si r<0 ligne polygonale extremite v[i] origine v[i+r] (r<0)
      for (k = 1; k < l; k++) {
        prec = v[k - 1];
        var cur = v[k];
        var preccouleur = prec[3] >> 11; // -> FIXME colors
        var curcouleur = prec[3] >> 11; // -> FIXME colors
        if (cur[5].length) {
          ctx.font = cur[4] + 'px serif';
          ctx.strokeStyle = ctx.fillStyle = UI.turtle_color(curcouleur);
          ctx.fillText(cur[5], turtlezoom * (cur[0] - turtlex), h - turtlezoom * (cur[1] - turtley));
          continue;
        }
        var radius = cur[4], precradius = prec[4];
        var x1 = Math.floor(turtlezoom * (prec[0] - turtlex) + .5),
            y1 = Math.floor(turtlezoom * (prec[1] - turtley) + .5),
            x2 = Math.floor(turtlezoom * (cur[0] - turtlex) + .5),
            y2 = Math.floor(turtlezoom * (cur[1] - turtley) + .5);
        if (radius > 0) {
          var r = radius & 0x1ff, theta1, theta2, rempli, x, y, R, angle;
          theta1 = prec[2]+ ((radius >> 9) & 0x1ff);
          theta2 = prec[2] + ((radius >> 18) & 0x1ff);
          rempli = (radius >> 27) & 1;
	  var seg = (radius >> 28) & 1;
          R = Math.floor(turtlezoom * r + .5);
          angle1 = Math.PI / 180 * (theta1 - 90);
          angle2 = Math.PI / 180 * (theta2 - 90);
          x = Math.floor(turtlezoom * (cur[0] - turtlex - r * Math.cos(angle2)) + .5);
          y = Math.floor(turtlezoom * (cur[1] - turtley - r * Math.sin(angle2)) + .5);
          ctx.beginPath();
	  if (seg)
            ctx.moveTo(x2, h - y2);
	  else {
            ctx.moveTo(x, h - y);
            ctx.lineTo(x2, h - y2);
	  }
	  //console.log(x,y,x1,y1,angle1,angle2);
          ctx.arc(x, h - y, R, -angle2,-angle1);
          ctx.closePath();
          ctx.strokeStyle = ctx.fillStyle = UI.turtle_color(curcouleur);
          if (rempli)
            ctx.fill();
          else
            ctx.stroke();
          continue;
        }
        if (prec[3] & 1) {
          ctx.strokeStyle = ctx.fillStyle = UI.turtle_color(preccouleur);
          ctx.beginPath();
          ctx.moveTo(x1, h - y1);
          ctx.lineTo(x2, h - y2);
          ctx.closePath();
          ctx.stroke();
        }
        if (radius < -1 && k + radius >= 0) {
          ctx.strokeStyle = ctx.fillStyle = UI.turtle_color(curcouleur);
          ctx.beginPath();
          var x0 = Math.floor(turtlezoom * (cur[0] - turtlex) + .5), y0 = Math.floor(turtlezoom * (cur[1] - turtley) + .5);
          //console.log('begin',x0,y0);
          ctx.moveTo(x0, h - y0);
          for (var i = -1; i >= radius; i--) {
            prec = v[k + i];
            var x = Math.floor(turtlezoom * (prec[0] - turtlex) + .5);
            var y = Math.floor(turtlezoom * (prec[1] - turtley) + .5);
            //console.log(i,x,y);
            ctx.lineTo(x, h - y);
          }
          //console.log('end',x0,y0);
          //ctx.lineTo(x0,h-y0);
          ctx.closePath();
          ctx.fill(); // automatically close path
        }
      }
      var cur = v[l - 1];
      if (cur[3] & 2) {
        // dessin de la tortue
        var x = Math.floor(turtlezoom * (cur[0] - turtlex) + .5);
        var y = Math.floor(turtlezoom * (cur[1] - turtley) + .5);
        var cost = Math.cos(cur[2] * Math.PI / 180);
        var sint = Math.sin(cur[2] * Math.PI / 180);
        var turtle_length = (cur[3] >> 3) & 0xff;
        var Dx = Math.floor(turtlezoom * turtle_length * cost / 2 + .5);
        var Dy = Math.floor(turtlezoom * turtle_length * sint / 2 + .5);
        //console.log('tortue',cur,w,h,turtlezoom,x,y,Dx,Dy);
        ctx.strokeStyle = ctx.fillStyle = UI.turtle_color(curcouleur);
        ctx.beginPath();
        ctx.moveTo(x + Dy, h - (y - Dx));
        ctx.lineTo(x - Dy, h - (y + Dx));
        ctx.closePath();
        ctx.stroke();
        if (!(cur[3] & 1))
          ctx.strokeStyle = ctx.fillStyle = UI.turtle_color(curcouleur + 1);
        ctx.beginPath();
        ctx.moveTo(x + Dy, h - (y - Dx));
        ctx.lineTo(x + 3 * Dx, h - (y + 3 * Dy));
        ctx.closePath();
        ctx.stroke();
        ctx.beginPath();
        ctx.moveTo(x - Dy, h - (y + Dx));
        ctx.lineTo(x + 3 * Dx, h - (y + 3 * Dy));
        ctx.closePath();
        ctx.stroke();
      }
    }
  }
}; // closing UI={
