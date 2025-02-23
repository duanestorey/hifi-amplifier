width = 98;
height = 60;
depth = 3;
$fn = 128;
hole_size = 2.5/2;
hole_offset = 2.5;

difference() {
  cube( [ width, depth, height ] );
    
    translate( [ (width-75)/2, -1, (height-25)/2 ] ) { 
        cube( [ 75, 5, 25] );
    }
   
    // Subwoofer
    translate( [ hole_offset, 4, hole_offset ] ) { 
        rotate( [ 90, 0, 0] ) {
            cylinder( 4, hole_size, hole_size );
        }
    }  
    
    translate( [ width - hole_offset, 4, height - hole_offset ] ) { 
        rotate( [ 90, 0, 0] ) {
            cylinder( 4, hole_size, hole_size );
        }
    }  
        
    translate( [ hole_offset, 4, height - hole_offset ] ) { 
        rotate( [ 90, 0, 0] ) {
            cylinder( 4, hole_size, hole_size );
        }
    }  
    
    translate( [ width - hole_offset, 4, hole_offset ] ) { 
        rotate( [ 90, 0, 0] ) {
            cylinder( 4, hole_size, hole_size );
        }
    }  
 
}

