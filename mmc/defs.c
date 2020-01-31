/**
 * State of the Art Matrix Midi Controller
 * 
 * License: Apache v2.0
 * Site: https://state-of-the-art.io/projects/matrix-midi-controller/
 */
#include <usb_names.h>

#define MANUFACTURER_NAME {'S','t','a','t','e',' ','o','f',' ','t','h','e',' ','A','r','t'}
#define MANUFACTURER_NAME_LEN 16
#define PRODUCT_NAME {'S','o','t','A',' ','M','a','t','r','i','x',' ','M','I','D','I',' ','C','o','n','t','r','o','l','l','e','r'}
#define PRODUCT_NAME_LEN 27

struct usb_string_descriptor_struct usb_string_manufacturer_name = {
        2 + MANUFACTURER_NAME_LEN * 2,
        3,
        MANUFACTURER_NAME
};

struct usb_string_descriptor_struct usb_string_product_name = {
        2 + PRODUCT_NAME_LEN * 2,
        3,
        PRODUCT_NAME
};
