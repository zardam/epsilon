EXTRA_DIST = xcasmenu xcasex keywords aide_cas

the_LANG = zh

files = aide_cas xcasmenu xcasex keywords # html_mall html_mtt html_vall

install-exec-hook: $(files)
	../../mkinstalldirs $(DESTDIR)$(prefix)/share/giac/doc/$(the_LANG)
	/bin/cp -f $(files) $(DESTDIR)$(prefix)/share/giac/doc/$(the_LANG)

