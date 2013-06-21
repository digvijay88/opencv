set(the_description "Barcode")
ocv_define_module(barcode opencv_core opencv_imgproc opencv_flann OPTIONAL opencv_highgui)

set_target_properties(${zxing} PROPERTIES LINK_INTERFACE_LIBRARIES "zxing")
