width = 433;
height = 80;
depth = 1;

margin_lr = 20;

$fn = 64;


rca_dist = 18;

offset = 30;
y2 = 25;
y1 = y2 + offset;

y_mid = 40;

subwoofer_input_x = 30;
spdif_input_x = 60;
xlr_input_x = 230;
rca_input_x = 110;
usb_input_x = 410;

reset_size = 4;
usb_size = 10;
xlr_size = 12;
rca_size = 4;
xlr_radius = 12;
xlr_diam = xlr_radius;
m3_holesize = 3.4/2;


difference() {
  cube( [ width, depth, height ] );
    // Subwoofer
    translate( [ subwoofer_input_x, 1, y2 ] ) { 
        rotate( [ 90, 0, 0] ) {
            cylinder( 2, rca_size, rca_size );
        }
    }   
   
   // IR receiver remote
   translate( [ subwoofer_input_x, 1, y1 ] ) { 
        rotate( [ 90, 0, 0] ) {
            cylinder( 2, 3.1, 3.1 );
        }
    }    
    
    // XLR jack
     translate( [ xlr_input_x, 1, y2 ] ) { 
        rotate( [ 90, 0, 0] ) {
            cylinder( 2, xlr_size, xlr_size );
        }
    }
    
     translate( [ xlr_input_x + 50, 1, y2  ] ) { 
        rotate( [ 90, 0, 0] ) {
            cylinder( 2, xlr_size, xlr_size );
        }
    }
    
     // Audio L/R out jack
    translate( [ xlr_input_x + rca_dist/1.5, 1, y1 ] ) { 
        rotate( [ 90, 0, 0] ) {
            cylinder( 2, rca_size, rca_size );
        }
    }
    
    translate( [ xlr_input_x - rca_dist/1.5, 1, y1 ] ) { 
        rotate( [ 90, 0, 0] ) {
            cylinder( 2, rca_size, rca_size );
        }
    }
    
    translate( [ xlr_input_x + 50 + rca_dist/1.5, 1, y1  ] ) { 
        rotate( [ 90, 0, 0] ) {
            cylinder( 2, rca_size, rca_size );
        }
    }
    
    translate( [ xlr_input_x + 50 - rca_dist/1.5, 1, y1  ] ) { 
        rotate( [ 90, 0, 0] ) {
            cylinder( 2, rca_size, rca_size );
        }
    }
   
   
    // SPDIF inputs
    translate( [ spdif_input_x, 0, y1 ] ) { 
        rotate( [ 90, 0, 0] ) {
            cylinder( 2, rca_size, rca_size );
        }
    }
    
    translate( [ spdif_input_x, 0, y2 ] ) { 
        rotate( [ 90, 0, 0] ) {
            cylinder( 2, rca_size, rca_size );
        }
    }
    
    translate( [ spdif_input_x + rca_dist, 1, y1 ] ) { 
        rotate( [ 90, 0, 0 ] ) {
            cylinder( 2, rca_size, rca_size );
        }
    }
    
    translate( [ spdif_input_x + rca_dist, 1, y2 ] ) { 
        rotate( [ 90, 0, 0] ) {
            cylinder( 2, rca_size, rca_size );
        }
    } 
    
    
    // RCA inputs
    translate( [ rca_input_x, 0, y1 ] ) { 
        rotate( [ 90, 0, 0] ) {
            cylinder( 2, rca_size, rca_size );
        }
    }
    
    translate( [ rca_input_x, 0, y2 ] ) { 
        rotate( [ 90, 0, 0] ) {
            cylinder( 2, rca_size, rca_size );
        }
    }
    
    translate( [ rca_input_x + rca_dist, 1, y1 ] ) { 
        rotate( [ 90, 0, 0 ] ) {
            cylinder( 2, rca_size, rca_size );
        }
    }
    
    translate( [ rca_input_x + rca_dist, 1, y2 ] ) { 
        rotate( [ 90, 0, 0] ) {
            cylinder( 2, rca_size, rca_size );
        }
    } 
 
     translate( [ rca_input_x + rca_dist*2, 1, y1 ] ) { 
        rotate( [ 90, 0, 0] ) {
            cylinder( 2, rca_size, rca_size );
        }
    }
    
    translate( [ rca_input_x + rca_dist*2, 1, y2 ] ) { 
        rotate( [ 90, 0, 0] ) {
            cylinder( 2, rca_size, rca_size );
        }
    }  
     translate( [ rca_input_x + rca_dist*3, 1, y1 ] ) { 
        rotate( [ 90, 0, 0] ) {
            cylinder( 2, rca_size, rca_size );
        }
    }
    
    translate( [ rca_input_x + rca_dist*3, 1, y2 ] ) { 
        rotate( [ 90, 0, 0] ) {
            cylinder( 2, rca_size, rca_size );
        }
    } 
    
     translate( [ rca_input_x + rca_dist*4, 1, y1 ] ) { 
        rotate( [ 90, 0, 0] ) {
            cylinder( 2, rca_size, rca_size );
        }
    }
    
    translate( [ rca_input_x + rca_dist*4, 1, y2 ] ) { 
        rotate( [ 90, 0, 0] ) {
            cylinder( 2, rca_size, rca_size );
        }
    } 
    
    // usb
    translate( [ usb_input_x, 1, y2 ] ) { 
        rotate( [ 90, 0, 0] ) {
            cylinder( 2, usb_size, usb_size );
        }
    } 
    
    // reset
    translate( [ usb_input_x, 1, y1 ] ) { 
        rotate( [ 90, 0, 0] ) {
            cylinder( 2, reset_size, reset_size );
        }
    } 
}

