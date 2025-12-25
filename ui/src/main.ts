import './styles/global.css';
import { WebSocketService } from './services/websockets';
import type { WebMessage } from './types/messages';

const ws = new WebSocketService("ws://" + location.hostname + "/ws");

const app = document.getElementById('app')!;
app.innerHTML = `
  <h1>ESPSynth Test</h1>
  <div>
    <label for="slider">Amplitude: <span id="value">0.50</span></label>
    <input type="range" id="slider" min="0" max="1" step="0.01" value="0.5">
  </div>
`;

const slider = document.getElementById('slider') as HTMLInputElement;
const valueDisplay = document.getElementById('value')!;

// Send slider value to ESP32
slider.addEventListener('input', () => {
  const value = parseFloat(slider.value);
  valueDisplay.textContent = value.toFixed(2);
  
  ws.send({
    type: 'set',
    id: 'synth.amplitude',
    value: value
  });
  
  console.log('Sent to ESP32:', { type: 'set', id: 'amplitude', value });
});

ws.onMessage('synth.amplitude', (msg: WebMessage) => {
  console.log('Received from ESP32:', msg);
  if (msg.value !== undefined) {
    slider.value = msg.value.toString();
    valueDisplay.textContent = msg.value.toFixed(2);
  }
});