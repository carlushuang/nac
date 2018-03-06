
# op c registry file
file(GLOB tmp op_c_*.cc)
set(nac_SRC ${nac_SRC} ${tmp})
set(nac_SRC ${nac_SRC} PARENT_SCOPE)
