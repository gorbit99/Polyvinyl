idf_component_register(SRCS
    "vqf.cpp"
    INCLUDE_DIRS "."
)

if(CONFIG_USE_SINGLE_PRECISION)
    target_compile_definitions(${COMPONENT_LIB} PUBLIC VQF_SINGLE_PRECISION)
endif()

if(CONFIG_NO_MOTION_BIAS_ESTIMATION)
    target_compile_definitions(${COMPONENT_LIB} PUBLIC
    VQF_NO_MOTION_BIAS_ESTIMATION)
endif()
