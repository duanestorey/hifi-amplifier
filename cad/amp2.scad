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


/*
// I2S board (violet)
translate( [ margin_lr, margin_lr + 70, 30 ] ) {
  color( "violet" )
  cube( [ 85, 55, 4 ] );   
}
*/


/*
// subwoofer
translate( [ margin_lr, margin_lr + 210, 10 ] ) {
    color( "pink" )
  cube( [ 85, 55, 4 ] );   
}
*/

// SPDIF (cyan)
translate( [ margin_lr, margin_lr + 70, 10 ] ) {
    color( "cyan" )
  cube( [ 85, 55, 4 ] );   
}


// microprocessor
translate( [ margin_lr, margin_lr, 10 ] ) {
  color( "brown" )
  cube( [ 85, 55, 4 ] );   
}



// Column Two

// vinyl (RED)
translate( [ 120, margin_lr + 70, 10 ] ) {
  color( "red" )
  cube( [ 85, 55, 4 ] );   
}

// ADC board (indigo)
translate( [ 120, margin_lr + 70, 30 ] ) {
  color( "indigo" )
  cube( [ 85, 55, 4 ] );   
}


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
translate( [ margin_lr + 100, margin_lr, 30 ] ) {
  color( "#000080" )
  cube( [ 85, 55, 4 ] );   
}

// FTDI (purple)
translate( [ margin_lr + 100, margin_lr, 10 ] ) {
  color( "purple" )
  cube( [ 85, 55, 4 ] );   
}

// POWER ROW

// digital/ananlog power suppy
translate( [ 240, margin_lr, 10 ] ) {
  color( "Lavender" )
  cube( [ 65, 95, 4 ] );   
}
    
// power suppy
translate( [ width - margin_lr - 95, margin_lr, 10 ] ) {
  cube( [ 95, 95, 4 ] );   
}

// rectifier (pink)
translate( [ width - margin_lr - 35, depth - 160 - margin_lr, 10 ] ) {
  color( "pink" )
  cube( [ 35, 65, 4 ] );   
}

// ground lift (green)
translate( [ width - margin_lr - 35, depth - 80 - margin_lr, 10 ] ) {
  color( "green" )
  cube( [ 35, 65, 4 ] );   
}



// toroid
translate( [ width - margin_lr - 180, depth - margin_lr - 10 -130, 0 ] ) {
  cube( [ 124, 124, 41 ] );   
}
       
