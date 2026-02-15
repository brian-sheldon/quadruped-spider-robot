

const char pageJs[] PROGMEM = R"=====(
function start() {
}
function move(dir) { fetch('/cmd?go=' + dir).catch(console.log); }
function stop() { fetch('/cmd?stop=1').catch(console.log); }
function pose(name) { fetch('/cmd?pose=' + name).catch(console.log); }
function openSettings() {
  fetch('/getSettings').then(r => r.json()).then(data => {
    document.getElementById('frameDelay').value = data.frameDelay;
    document.getElementById('walkCycles').value = data.walkCycles;
    document.getElementById('motorCurrentDelay').value = data.motorCurrentDelay;
    document.getElementById('faceFps').value = data.faceFps;
    document.getElementById('settingsPanel').style.display = 'block';
  });
}
function closeSettings() { document.getElementById('settingsPanel').style.display = 'none'; }
function saveSettings() {
  const fd = document.getElementById('frameDelay').value;
  const wc = document.getElementById('walkCycles').value;
  const mcd = document.getElementById('motorCurrentDelay').value;
  const fps = document.getElementById('faceFps').value;
  fetch(`/setSettings?frameDelay=${fd}&walkCycles=${wc}&motorCurrentDelay=${mcd}&faceFps=${fps}`).then(() => closeSettings());
}
)=====";

