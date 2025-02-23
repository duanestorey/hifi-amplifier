width = 435;
height = 82;
depth = 311;

margin_lr = 20;

$fn = 64;

difference() {
  cube( [ width, depth, height ] );
            
  translate( [ 2, 2, 2 ] ) {
    cube( [ width - 4, depth - 4, height + 10 ] );
  }
}

// Column One 

// PCM5142/DAC
translate( [ margin_lr, 220 + margin_lr - 10, 10 ] ) {
  color( "black" )
  cube( [ 85, 55, 4 ] );   
}

// DSP
translate( [ margin_lr, 150 + margin_lr - 10, 10 ] ) {
  color( "orange" )
  cube( [ 85, 55, 4 ] );   
}


// SPDIF (cyan)
translate( [ margin_lr, margin_lr + 70, 10 ] ) {
    color( "cyan" )
  cube( [ 85, 55, 4 ] );   
}


// Dolby
translate( [ margin_lr, margin_lr, 10 ] ) {
    color( "brown" )
  cube( [ 85, 55, 4 ] );   
}


// Column Two

// inputs (grey)
translate( [ 120, margin_lr + 210, 10 ] ) {
  color( "grey" )
  cube( [ 85, 55, 4 ] );   
}

// channel sel (Yellow)
translate( [ 120, margin_lr + 140, 10 ] ) {
  color( "yellow" )
  cube( [ 85, 55, 4 ] );   
}

// GPIO extender (blue)
translate( [ margin_lr + 100, margin_lr + 70, 10 ] ) {
  color( "#000080" )
  cube( [ 85, 55, 4 ] );   
}

// microprocessor
translate( [ margin_lr + 100, margin_lr, 10 ] ) {
  color( "brown" )
  cube( [ 85, 55, 4 ] );   
}

// Column Three

// vinyl (RED)
translate( [ 220, margin_lr + 140, 10 ] ) {
  color( "red" )
  cube( [ 85, 55, 4 ] );   
}

// ADC board (indigo)
translate( [ 220, margin_lr + 70, 10 ] ) {
  color( "pink" )
  cube( [ 85, 55, 4 ] );   
}

// FTDI (purple)
translate( [ 220, margin_lr, 10 ] ) {
  color( "purple" )
  cube( [ 85, 55, 4 ] );   
}

// Column Four

// Split Distributor
translate( [ 320, margin_lr, 10 ] ) {
  color( "indigo" )
  cube( [ 85, 55, 4 ] );   
}

// Monitor
translate( [ 320, margin_lr + 70, 10 ] ) {
  color( "orange" ) 
  cube( [ 85, 55, 4 ] );  
   
}

// Multi PSU
translate( [ 320, margin_lr + 140, 10 ] ) {
  color( "red" )
  cube( [ 85, 55, 4 ] );   
}


// Entry
translate( [ 320, margin_lr + 210, 10 ] ) {
  color( "white" )
  cube( [ 85, 55, 4 ] );   
}
