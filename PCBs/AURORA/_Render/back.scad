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

subwoofer_input_x = 70;
spdif_input_x = 25;
xlr_input_x = 250;
rca_input_x = 120;
usb_input_x = 400;

reset_size = 4;
usb_size = 24/2;
xlr_size = 12;
rca_size = 4;
xlr_radius = 12;
xlr_diam = xlr_radius;
m3_holesize = 3.2/2;
rpsma_size = 7.5/2;


difference() {
  cube( [ width, depth, height ] );
    // Subwoofer
    translate( [ subwoofer_input_x + 20, 1, y2] ) { 
        rotate( [ 90, 0, 0] ) {
            cylinder( 2, rca_size, rca_size );
        }
    }   
   
   // IR receiver remote
   translate( [ subwoofer_input_x + 20, 1, y1] ) { 
        rotate( [ 90, 0, 0] ) {
            cylinder( 2, 3.1, 3.1 );
        }
    }   
    translate( [ subwoofer_input_x, 1, (y2)  ] ) { 
        rotate( [ 90, 0, 0] ) {
            cylinder( 2, rpsma_size, rpsma_size );
        }
    }     
    
    // XLR jack
     translate( [ xlr_input_x, 1, y2 - 2.5 ] ) { 
        rotate( [ 90, 0, 0] ) {
            cylinder( 2, xlr_size, xlr_size );
        }
    }
    
     translate( [ xlr_input_x + 35, 1, y2 - 2.5 ] ) { 
        rotate( [ 90, 0, 0] ) {
            cylinder( 2, xlr_size, xlr_size );
        }
    }
    
    // Audio L/R out jack
    translate( [ xlr_input_x, 1, y1 + 2.5] ) { 
        rotate( [ 90, 0, 0] ) {
            cylinder( 2, xlr_size, xlr_size );
        }
    }
    
    translate( [ xlr_input_x + 35,  1, y1 + 2.5  ] ) { 
        rotate( [ 90, 0, 0] ) {
            cylinder( 2, xlr_size, xlr_size );
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
    
         translate( [ rca_input_x + rca_dist*5, 1, y1 ] ) { 
        rotate( [ 90, 0, 0] ) {
            cylinder( 2, rca_size, rca_size );
        }
    }
    
    translate( [ rca_input_x + rca_dist*5, 1, y2 ] ) { 
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

