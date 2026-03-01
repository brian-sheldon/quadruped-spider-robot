

const char pageJs[] PROGMEM = R"=====(
function log( s ) {
  let lf = '<br>';
  let contents = $('#log').html();
  $('#log').html( s + lf + contents );
}
function toolhideshow() {
  if ( $('#log').hasClass( 'hidden' ) ) {
    $('.tool').removeClass( 'hidden' );
  } else {
    $('.tool').addClass( 'hidden' );
  }
}

class Servo {
  constructor( index, label, angles, anglesLabel ) {
    this.index = index;
    this.label = label;
    this.angles = angles;
    this.anglesLabel = anglesLabel;
    this.angle = this.angles[0];
    this.angleMin = Math.min( ...angles );
    this.angleMax = Math.max( ...angles );
    this.hzMin = 550;
    this.hzMax = 2250;
    this._fetchServoStateComplete = false;
    this.buttonInit();
    log( this.state );
  }
  get fetchServoStateComplete() {
    return this._fetchServoStateComplete;
  }
  fetchServoStateJson() {
    let self = this;
    self._fetchServoStateComplete = false;
    fetch( '/getServoStateJson?servo=' + this.index )
      .then( res => res.json() )
      .then( data => {
        self._fetchServoStateComplete = true;
        log( JSON.stringify( data ) );
        let failed = false;
        if ( data.index != this.index ) failed = true;
        if ( data.label != this.label ) failed = true;
        if ( data.angleMin != this.angleMin ) failed = true;
        if ( data.angleMax != this.angleMax ) failed = true;
        if ( data.angle != this.angle ) failed = true;
        log( "server data matches web app data: " + ( ! failed ) );
      })
      .catch( error => {
        log( 'fetch error servo: ' + self.index + ' error: ' + error );
      });
  }
  get angle() {
    return this._angle;
  }
  set angle( angle ) {
    if ( angle >= this.angleMin && angle <= this.angleMax ) {
      this._angle = angle;
    }
  }
  get angleCurrIndex() {
    let i = this.angles.indexOf( this.angle );
    if ( i < 0 ) {
      i = 0;
    }
    return i;
  }
  get angleNextIndex() {
    return ( this.angleCurrIndex + 1 ) % this.angles.length;
  }
  get angleCurr() {
    return this.angles[ this.angleCurrIndex ];
  }
  get angleNext() {
    return this.angles[ this.angleNextIndex ];
  }
  get angleCurrLabel() {
    return this.anglesLabel[ this.angleCurrIndex ];
  }
  get angleNextLabel() {
    return this.anglesLabel[ this.angleNextIndex ];
  }
  angleToggle() {
    this.angle = this.angleNext;
    this.servoAngle();
  }
  servoAngle() {
    fetch( '/servoAngle?servo=' + this.index + '&angle=' + this.angle )
      .then( res => res.json() )
      .then( data => {
        log( JSON.stringify( data ) );
        
      })
      .catch( error => {
        log( 'fetch error: ' + error );
      });
  }
  get state() {
    let s = '';
    s += this.label;
    s += ' ' + this.angleCurrLabel + ' ' + this.angleCurr + ' -> ' + this.angleNext + ' ' + this.angleNextLabel;
    return s;
  }
  //
  get buttonLabel() {
    let curr = this.angleCurr;
    let next = this.angleNext;
    return this.label + ' ( ' + curr + ' -> ' + next + ' )';
  }
  buttonInit() {
    let self = this;
    let id = '#' + this.label;
    $(id).text( this.state );
    $(id).on( 'click', function() {
      self.angleToggle();
      $(id).text( self.state );
      log( self.state );
    });
  }
  utf8Bytes( ch ) {
    let byteArr = Array.from( new TextEncoder().encode( data.anglesLabel[0] ) );
    let s = '';
    for ( let i = 0; i < byteArr.length; i++ ) {
      s += byteArr[i].toString(16);
    }
    return s;
  }
}

function start() {
  log( 'starting ...' );
  initServoButtons();
}

function initServoButtons() {
  let w = '\u2190';
  let up = '\u2191';
  let n = up;
  let e = '\u2192';
  let down = '\u2193';
  let s = down;
  let nw = '\u2196';
  let ne = '\u2197';
  let sw = '\u2199';
  let se = '\u2198';
  let servos = [];
  let i = 0;
  // femors
  servos[i] = new Servo( i++, 'R1', [ 135, 90, 180 ], [ ne, e, n ] );
  servos[i] = new Servo( i++, 'R2', [ 45, 90, 0 ], [ se, e, s ] );
  servos[i] = new Servo( i++, 'L1', [ 45, 90, 0 ], [ nw, w, n ] );
  servos[i] = new Servo( i++, 'L2', [ 135, 90, 180 ], [ sw, w, s ] );
  // feet
  servos[i] = new Servo( i++, 'R4', [ 0, 45, 90, 135, 180 ], [ down, se, e, ne, up ] );
  servos[i] = new Servo( i++, 'R3', [ 180, 135, 90, 45, 0 ], [ down, se, e, ne, up ] );
  servos[i] = new Servo( i++, 'L3', [ 0, 45, 90, 135, 180 ], [ down, sw, w, nw, up ] );
  servos[i] = new Servo( i++, 'L4', [ 180, 135, 90, 45, 0 ], [ down, sw, w, nw, up ] );

  // compare with server
  // just using to check a few things for now
  // turning off
  if ( false ) {
    i = 0;
    let doNext = true;
    let id = setInterval( function() {
      if ( doNext ) {
        doNext = false;
        log( 'send fetchServoStateJson request servo: ' + i );
        servos[i].fetchServoStateJson();
        //log( servos[i].fetchServoStateComplete );
      } else {
        if ( servos[i].fetchServoStateComplete ) {
          log( 'doing next ...' );
          i++;
          if ( i > 7 ) {
            clearInterval( id );
          } else {
            doNext = true;
          }
        }
      }
    }, 10 );
  }
}

function move(dir) {
  fetch( '/cmd?go=' + dir )
    .catch(log);
}
function stop() {
  fetch( '/cmd?stop=1' )
    .catch(log);
}
function pose(name) {
  fetch( '/cmd?pose=' + name )
    .then( r => r.text() )
    .then( data => {
      log( data );
    })
    .catch(log);
}
function openSettings() {
  fetch( '/getSettings' )
    .then( r => {
      r.json();
    })
    .then(data => {
      document.getElementById('frameDelay').value = data.frameDelay;
      document.getElementById('walkCycles').value = data.walkCycles;
      document.getElementById('motorCurrentDelay').value = data.motorCurrentDelay;
      document.getElementById('faceFps').value = data.faceFps;
      document.getElementById('settingsPanel').style.display = 'block';
    }
  );
}
function closeSettings() {
  document.getElementById('settingsPanel')
    .style.display = 'none';
}
function saveSettings() {
  const fd = document.getElementById('frameDelay').value;
  const wc = document.getElementById('walkCycles').value;
  const mcd = document.getElementById('motorCurrentDelay').value;
  const fps = document.getElementById('faceFps').value;
  fetch( `/setSettings?frameDelay=${fd}&walkCycles=${wc}&motorCurrentDelay=${mcd}&faceFps=${fps}` )
    .then( () => {
      closeSettings();
    }
  );
}
)=====";

