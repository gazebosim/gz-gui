set (translations
  translation_en.ts
  translation_es.ts
  translation_pt.ts
)
set(translations ${translations} PARENT_SCOPE)

QT5_ADD_TRANSLATION(translations_qm ${translations})
add_custom_target(translations ALL DEPENDS ${translations_qm})
