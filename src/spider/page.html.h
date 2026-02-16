//#pragma once

//#include <Arduino.h>

// ======================================================================
// --- WEB INTERFACE HTML ---
// ======================================================================

const char pageHtml[] PROGMEM = R"=====(
<!DOCTYPE HTML><html>
  <head>
    <title>Robot AP Controller</title>
    <meta http-equiv="Cache-Control" content="no-cache, no-store, must-revalidate"/>
    <meta http-equiv="Pragma" content="no-cache"/>
    <meta http-equiv="Expires" content="0"/>
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <link rel="stylesheet" href="page.css">
    <script src="https://ajax.googleapis.com/ajax/libs/jquery/3.7.1/jquery.min.js"></script>
    <script src="page.js"></script>
  </head>
  <body onload="start();">
    <h2>Sesame Controller</h2>
    
    <div class="dpad-container">
      <div class="dpad">
        <div class="spacer"></div>
        <button onmousedown="move('forward')" onmouseup="stop()" ontouchstart="move('forward')" ontouchend="stop()">&#9650;</button>
        <div class="spacer"></div>
        
        <button onmousedown="move('left')" onmouseup="stop()" ontouchstart="move('left')" ontouchend="stop()">&#9664;</button>
        <button onmousedown="move('backward')" onmouseup="stop()" ontouchstart="move('backward')" ontouchend="stop()">&#9660;</button>
        <button onmousedown="move('right')" onmouseup="stop()" ontouchstart="move('right')" ontouchend="stop()">&#9654;</button>
      </div>
    </div>

    <button class="btn-stop-all" onclick="stop()">STOP ALL</button>
    
    <div class="grid">
      <button class="btn-pose" onclick="pose('rest')">Rest</button>
      <button class="btn-pose" onclick="pose('stand')">Stand</button>
      <button class="btn-pose" onclick="pose('wave')">Wave</button>
      <button class="btn-pose" onclick="pose('dance')">Dance</button>
      <button class="btn-pose" onclick="pose('swim')">Swim</button>
      <button class="btn-pose" onclick="pose('point')">Point</button>
      <button class="btn-pose" onclick="pose('pushup')">Pushup</button>
      <button class="btn-pose" onclick="pose('bow')">Bow</button>
      <button class="btn-pose" onclick="pose('cute')">Cute</button>
      <button class="btn-pose" onclick="pose('freaky')">Freaky</button>
      <button class="btn-pose" onclick="pose('worm')">Worm</button>
      <button class="btn-pose" onclick="pose('shake')">Shake</button>
      <button class="btn-pose" onclick="pose('shrug')">Shrug</button>
      <button class="btn-pose" onclick="pose('dead')">Dead</button>
      <button class="btn-pose" onclick="pose('crab')">Crab</button>
      <button class="btn-pose tool" onclick="pose('test')">test</button>
    </div>
    <h3 class="tool">The Femurs</h3>
    <div class="grid">
      <button id="L1" class="btn-pose tool">L1</button>
      <button id="R1" class="btn-pose tool">R1</button>
      <button id="L2" class="btn-pose tool">L2</button>
      <button id="R2" class="btn-pose tool">R2</button>
    </div>
    <h3 class="tool">The Feet</h3>
    <div class="grid">
      <button id="L3" class="btn-pose tool">L3</button>
      <button id="R3" class="btn-pose tool">R3</button>
      <button id="L4" class="btn-pose tool">L4</button>
      <button id="R4" class="btn-pose tool">R4</button>
    </div>
    
    <button class="btn-settings" onclick="openSettings()">Settings</button>
    <button class="btn-loghideshow" onclick="toolhideshow()">Tools (show/hide)</button>

    <div id="settingsPanel" class="settings-panel">
      <div class="settings-content">
        <h3>Settings</h3>
        <label>Frame Delay (ms):</label>
        <input type="number" id="frameDelay">
        <label>Walk Cycles:</label>
        <input type="number" id="walkCycles">
        <label>Motor Current Delay (ms):</label>
        <input type="number" id="motorCurrentDelay">
        <label>Face FPS:</label>
        <input type="number" id="faceFps">
        <button class="btn-save" onclick="saveSettings()">Save Settings</button>
        <button class="btn-close" onclick="closeSettings()">Close</button>
      </div>
    </div>
    
    <div id="log" class="tool"></div>
    
  </body>
</html>
)=====";
