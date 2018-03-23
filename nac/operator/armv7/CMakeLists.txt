#
file(GLOB tmp op_*.cc)
set(_nac_ARMV7_SRC ${_nac_ARMV7_SRC} ${tmp})
set(_nac_ARMV7_SRC ${_nac_ARMV7_SRC} PARENT_SCOPE)