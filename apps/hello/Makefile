apps += Hello::App
app_headers += apps/hello/app.h

app_src += $(addprefix apps/hello/,\
  app.cpp \
  hello_controller.cpp \
  hello_view.cpp \
)

app_images += apps/hello/hello_icon.png

i18n_files += $(addprefix apps/hello/,\
	base.de.i18n\
	base.en.i18n\
	base.es.i18n\
	base.fr.i18n\
	base.pt.i18n\
)

$(eval $(call depends_on_image,apps/hello/app.cpp,apps/hello/hello_icon.png))
