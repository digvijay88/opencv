#<<<<<<< HEAD
set(the_description "Barcode")
ocv_define_module(barcode opencv_core opencv_highgui opencv_imgproc zxing boost_system boost_filesystem)
#ocv_define_module(barcode opencv_core opencv_highgui opencv_imgproc)

#set_target_properties(${the_module} PROPERTIES LINK_INTERFACE_LIBRARIES "zxing boost_system boost_filesystem")
#set_target_properties(${the_module} PROPERTIES LINK_INTERFACE_LIBRARIES "boost_system")
#set_target_properties(${the_module} PROPERTIES LINK_INTERFACE_LIBRARIES "boost_filesystem")
#=======
#set(the_description "Barcode Framework")
#ocv_define_module(barcode opencv_imgproc)
#>>>>>>> pr/1013
