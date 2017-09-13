set (translations
  translation_en.ts
  translation_es.ts
  translation_pt.ts
)

# ts -> qm
QT5_ADD_TRANSLATION(translations_qm ${translations})
add_custom_target(translations ALL DEPENDS ${translations_qm})

# Copy file to binary path so it is close to the generated qm files
set(lang_qrc "languages.qrc")
configure_file(${lang_qrc} ${lang_qrc} COPYONLY)

# Propagate variables to parent scope
set(translations ${translations} PARENT_SCOPE)
set(lang_qrc ${lang_qrc} PARENT_SCOPE)
