if (WITH_DOC_LATEX)
  file(GLOB DOCUMENT_TEX ${CMAKE_CURRENT_BINARY_DIR}/*.tex)
  if (EXISTS ${DOCUMENT_TEX})
    get_filename_component(DOCUMENT ${DOCUMENT_TEX} NAME_WE)
    if (LATEX_COMPILER)
      add_custom_command( 
	OUTPUT    ${CMAKE_CURRENT_BINARY_DIR}/${DOCUMENT}.dvi
	COMMAND   ${LATEX_COMPILER}
	ARGS      ${CMAKE_CURRENT_BINARY_DIR}/${DOCUMENT}.tex
	DEPENDS   ${CMAKE_CURRENT_BINARY_DIR}/${DOCUMENT}.tex
	COMMAND   ${LATEX_COMPILER}
	ARGS      ${CMAKE_CURRENT_BINARY_DIR}/${DOCUMENT}.tex
	COMMENT   "Tex2dvi (two passes) : ${DOCUMENT}.tex"
	)
      if(DVIPS_CONVERTER)
	add_custom_command( 
	  OUTPUT    ${CMAKE_CURRENT_BINARY_DIR}/${DOCUMENT}.ps
	  COMMAND   ${DVIPS_CONVERTER}
	  ARGS      ${CMAKE_CURRENT_BINARY_DIR}/${DOCUMENT}.dvi
          -o ${CMAKE_CURRENT_BINARY_DIR}/${DOCUMENT}.ps
	  DEPENDS   ${CMAKE_CURRENT_BINARY_DIR}/${DOCUMENT}.dvi
	  COMMENT   "dvi2ps : ${DOCUMENT}.dvi"
	  )
	if(PS2PDF_CONVERTER)
	  add_custom_command( 
	    OUTPUT    ${CMAKE_CURRENT_BINARY_DIR}/${DOCUMENT}.pdf
	    COMMAND   ${PS2PDF_CONVERTER}
	    ARGS      ${CMAKE_CURRENT_BINARY_DIR}/${DOCUMENT}.ps
	    DEPENDS   ${CMAKE_CURRENT_BINARY_DIR}/${DOCUMENT}.ps
	    COMMENT   "ps2pdf : ${DOCUMENT}.ps"
	    )
	  set(TARGET_NAME doc-latex-${DOCUMENT})
	  message(STATUS "Add target ${TARGET_NAME}")
	  add_custom_target(${TARGET_NAME} ALL echo
	    DEPENDS   ${CMAKE_CURRENT_BINARY_DIR}/${DOCUMENT}.pdf
	    )
	  if(NOT TARGET doc-latex)
	    message(STATUS "Add target doc-latex")
	    add_custom_target(doc-latex ALL)
	  endif(NOT TARGET doc-latex)
	  add_dependencies(doc-latex ${TARGET_NAME})
	endif(PS2PDF_CONVERTER)
      endif(DVIPS_CONVERTER)
    endif(LATEX_COMPILER)
  else (EXISTS ${DOCUMENT_TEX})
    message(FATAL_ERROR "there is not latex file in this repertory : " ${CMAKE_CURRENT_BINARY_DIR})
  endif (EXISTS ${DOCUMENT_TEX})
endif (WITH_DOC_LATEX)
