//=========================================================================
// ESP32 Hydroponisch Systeem Controller
// Web interface HTML/CSS/JavaScript
//=========================================================================

#ifndef WEBUI_H
#define WEBUI_H

// De volledige HTML, CSS en JavaScript code voor de webinterface
// Deze constante wordt gebruikt in WebServer.cpp
const char* WEBUI_HTML = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title id="pageTitle">Hydroponisch Systeem Controller</title>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    body {
      font-family: Arial, sans-serif;
      margin: 0;
      padding: 20px;
      max-width: 800px;
      margin: 0 auto;
      color: #333;
    }
    .card {
      background-color: #fff;
      border-radius: 5px;
      box-shadow: 0 2px 5px rgba(0,0,0,0.1);
      padding: 20px;
      margin-bottom: 20px;
    }
    h1 {
      color: #2c3e50;
      font-size: 24px;
      margin-top: 0;
    }
    .status-indicator {
      display: inline-block;
      width: 15px;
      height: 15px;
      border-radius: 50%;
      margin-right: 10px;
    }
    .on {
      background-color: #2ecc71;
    }
    .off {
      background-color: #e74c3c;
    }
    table {
      width: 100%;
      border-collapse: collapse;
    }
    table, th, td {
      border: 1px solid #ddd;
    }
    th, td {
      padding: 10px;
      text-align: left;
    }
    th {
      background-color: #f2f2f2;
    }
    button {
      background-color: #3498db;
      color: white;
      border: none;
      padding: 10px 15px;
      border-radius: 4px;
      cursor: pointer;
      font-size: 14px;
      margin-right: 10px;
    }
    button:hover {
      background-color: #2980b9;
    }
    button.danger {
      background-color: #e74c3c;
    }
    button.danger:hover {
      background-color: #c0392b;
    }
    button.success {
      background-color: #2ecc71;
    }
    button.success:hover {
      background-color: #27ae60;
    }
    input, select {
      padding: 8px;
      border: 1px solid #ddd;
      border-radius: 4px;
      width: 100%;
    }
    .settings-row {
      margin-bottom: 10px;
    }
    .settings-label {
      display: inline-block;
      width: 180px;
      margin-bottom: 5px;
    }
    .tab {
      display: none;
    }
    .tab.active {
      display: block;
    }
    .tab-buttons {
      margin-bottom: 20px;
      display: flex;
      border-bottom: 1px solid #ddd;
    }
    .tab-button {
      background-color: #2ecc71;
      border: none;
      padding: 10px 20px;
      margin-right: 5px;
      cursor: pointer;
      border-radius: 5px 5px 0 0;
    }
    .tab-button:hover {
      background-color: #eee;
    }
    .tab-button.active {
      background-color: #3498db;
      color: white;
    }
    .temperature {
      font-size: 36px;
      font-weight: bold;
      text-align: center;
      margin: 20px 0;
    }
    .settings-save {
      margin-top: 15px;
    }
    .override-section {
      display: flex;
      align-items: center;
      justify-content: space-between;
      padding: 10px 0;
    }
    .night-mode {
      margin-top: 10px;
      padding: 5px;
      border-radius: 4px;
      background-color: #34495e;
      color: white;
      text-align: center;
    }
    .current-time {
      text-align: center;
      margin: 10px 0;
      font-size: 14px;
      color: #7f8c8d;
    }
    .version-info {
      text-align: center;
      margin-top: 20px;
      font-size: 12px;
      color: #95a5a6;
    }
    @media (max-width: 600px) {
      .settings-label {
        width: 100%;
      }
      .override-section {
        flex-direction: column;
        align-items: flex-start;
      }
      .override-section button {
        margin-top: 10px;
      }
    }
  </style>
</head>
<body>
  <div class="card">
    <h1 id="systemName">Hydroponisch Systeem</h1>
    <div class="temperature"><span id="temp">--</span>°C</div>
    <div class="current-time" id="dateTime">Tijd laden...</div>
    <div>
      <span class="status-indicator off" id="pumpIndicator"></span>
      <span id="pumpStatus">Pomp status: Laden...</span>
    </div>
    <div id="overrideStatus" style="margin-top: 10px; color: #e74c3c; display: none;">
      Handmatige modus actief
    </div>
    <div id="nightMode" class="night-mode" style="display: none;">
      Nachtmodus actief
    </div>
  </div>

  <div class="tab-buttons">
    <button class="tab-button active" onclick="showTab('control')">Bediening</button>
    <button class="tab-button" onclick="showTab('settings')">Instellingen</button>
  </div>

  <div id="control-tab" class="tab active card">
    <h2>Handmatige bediening</h2>
    <div class="override-section">
      <div>
        <button id="overrideOnBtn" class="success" onclick="activateOverride(true)">Pomp AAN</button>
        <button id="overrideOffBtn" class="danger" onclick="activateOverride(false)">Pomp UIT</button>
      </div>
      <div>
        <button id="cancelOverrideBtn" onclick="cancelOverride()" style="display: none;">Terug naar automatisch</button>
      </div>
    </div>
  </div>

  <div id="settings-tab" class="tab card">
    <h2>Systeeminstellingen</h2>
    
    <div class="settings-row">
      <div class="settings-label">Systeemnaam:</div>
      <input type="text" id="systeemnaam" maxlength="31" value="">
    </div>
    
    <h3>Temperatuurgrenzen</h3>
    <div class="settings-row">
      <div class="settings-label">Lage temperatuurgrens:</div>
      <input type="number" id="temp_laag_grens" min="0" max="40" step="0.5"> °C
    </div>
    <div class="settings-row">
      <div class="settings-label">Hoge temperatuurgrens:</div>
      <input type="number" id="temp_hoog_grens" min="0" max="40" step="0.5"> °C
    </div>
    
    <h3>Pompcycli instellingen</h3>
    <table>
      <tr>
        <th>Temperatuurbereik</th>
        <th>AAN (seconden)</th>
        <th>UIT (seconden)</th>
      </tr>
      <tr>
        <td>Laag (< <span id="laag_display">18</span>°C)</td>
        <td><input type="number" id="temp_laag_aan" min="0"></td>
        <td><input type="number" id="temp_laag_uit" min="0"></td>
      </tr>
      <tr>
        <td>Midden (<span id="midden_min_display">18</span>-<span id="midden_max_display">25</span>°C)</td>
        <td><input type="number" id="temp_midden_aan" min="0"></td>
        <td><input type="number" id="temp_midden_uit" min="0"></td>
      </tr>
      <tr>
        <td>Hoog (> <span id="hoog_display">25</span>°C)</td>
        <td><input type="number" id="temp_hoog_aan" min="0"></td>
        <td><input type="number" id="temp_hoog_uit" min="0"></td>
      </tr>
      <tr>
        <td>Nacht</td>
        <td><input type="number" id="nacht_aan" min="0"></td>
        <td><input type="number" id="nacht_uit" min="0"></td>
      </tr>
    </table>
    
    <div class="settings-save">
      <button onclick="saveSettings()">Instellingen opslaan</button>
    </div>
  </div>

  <div class="version-info">
    ESP32 Hydroponisch Systeem Controller v1.0.0<br>
    <a href="https://github.com/imoliamedia/AXISKOM-ESP32-Hydroponisch-Systeem-Controller" target="_blank">Axiskom GitHub Repository</a>
  </div>

  <script>
    // Globale variabelen
    let overrideActive = false;
    let statusRefreshInterval;
    let isNachtModus = false;
    
    // Pagina initialisatie
    document.addEventListener('DOMContentLoaded', function() {
      fetchStatus();
      fetchSettings();
      
      // Start auto-refresh
      statusRefreshInterval = setInterval(fetchStatus, 5000);
    });
    
    // Tab functionaliteit
    function showTab(tabName) {
      document.querySelectorAll('.tab').forEach(tab => {
        tab.classList.remove('active');
      });
      document.querySelectorAll('.tab-button').forEach(button => {
        button.classList.remove('active');
      });
      
      document.getElementById(tabName + '-tab').classList.add('active');
      document.querySelector(`.tab-button[onclick="showTab('${tabName}')"]`).classList.add('active');
    }
    
    // Status ophalen
    function fetchStatus() {
      fetch('/api/status')
        .then(response => response.json())
        .then(data => {
          document.getElementById('temp').textContent = data.temperature.toFixed(1);
          document.getElementById('pumpStatus').textContent = `Pomp status: ${data.pumpState ? 'AAN' : 'UIT'}`;
          document.getElementById('pumpIndicator').className = 
            data.pumpState ? 'status-indicator on' : 'status-indicator off';
          
          // Update tijdsweergave
          document.getElementById('dateTime').textContent = data.currentDateTime;
          
          // Update nachtmodus status
          isNachtModus = data.isNightMode;
          document.getElementById('nightMode').style.display = 
            isNachtModus ? 'block' : 'none';
          
          // Override status bijwerken
          overrideActive = data.overrideActive;
          document.getElementById('overrideStatus').style.display = 
            overrideActive ? 'block' : 'none';
          document.getElementById('cancelOverrideBtn').style.display = 
            overrideActive ? 'inline-block' : 'none';
        })
        .catch(error => {
          console.error('Fout bij het ophalen van status:', error);
          // Optioneel: toon een foutmelding aan de gebruiker
        });
    }
    
    // Instellingen ophalen
    function fetchSettings() {
      fetch('/api/settings')
        .then(response => response.json())
        .then(data => {
          // Vul de formuliervelden in
          document.getElementById('systeemnaam').value = data.systeemnaam;
          document.getElementById('temp_laag_grens').value = data.temp_laag_grens;
          document.getElementById('temp_hoog_grens').value = data.temp_hoog_grens;
          document.getElementById('temp_laag_aan').value = data.temp_laag_aan;
          document.getElementById('temp_laag_uit').value = data.temp_laag_uit;
          document.getElementById('temp_midden_aan').value = data.temp_midden_aan;
          document.getElementById('temp_midden_uit').value = data.temp_midden_uit;
          document.getElementById('temp_hoog_aan').value = data.temp_hoog_aan;
          document.getElementById('temp_hoog_uit').value = data.temp_hoog_uit;
          document.getElementById('nacht_aan').value = data.nacht_aan;
          document.getElementById('nacht_uit').value = data.nacht_uit;
          
          // Update pagina titel en labels
          document.getElementById('pageTitle').textContent = data.systeemnaam;
          document.getElementById('systemName').textContent = data.systeemnaam;
          
          // Update temperatuur grenzen in labels
          document.getElementById('laag_display').textContent = data.temp_laag_grens;
          document.getElementById('midden_min_display').textContent = data.temp_laag_grens;
          document.getElementById('midden_max_display').textContent = data.temp_hoog_grens;
          document.getElementById('hoog_display').textContent = data.temp_hoog_grens;
        })
        .catch(error => {
          console.error('Fout bij het ophalen van instellingen:', error);
          // Optioneel: toon een foutmelding aan de gebruiker
        });
    }
    
    // Instellingen opslaan
    function saveSettings() {
      const settings = {
        systeemnaam: document.getElementById('systeemnaam').value,
        temp_laag_grens: parseFloat(document.getElementById('temp_laag_grens').value),
        temp_hoog_grens: parseFloat(document.getElementById('temp_hoog_grens').value),
        temp_laag_aan: parseInt(document.getElementById('temp_laag_aan').value),
        temp_laag_uit: parseInt(document.getElementById('temp_laag_uit').value),
        temp_midden_aan: parseInt(document.getElementById('temp_midden_aan').value),
        temp_midden_uit: parseInt(document.getElementById('temp_midden_uit').value),
        temp_hoog_aan: parseInt(document.getElementById('temp_hoog_aan').value),
        temp_hoog_uit: parseInt(document.getElementById('temp_hoog_uit').value),
        nacht_aan: parseInt(document.getElementById('nacht_aan').value),
        nacht_uit: parseInt(document.getElementById('nacht_uit').value)
      };
      
      fetch('/api/settings', {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json',
        },
        body: JSON.stringify(settings)
      })
      .then(response => {
        if (!response.ok) {
          return response.text().then(text => { throw new Error(text) });
        }
        return response.json();
      })
      .then(data => {
        alert('Instellingen opgeslagen!');
        fetchSettings(); // Refresh instellingen
      })
      .catch(error => {
        console.error('Fout bij het opslaan van instellingen:', error);
        alert('Fout bij het opslaan van instellingen: ' + error.message);
      });
    }
    
    // Override activeren
    function activateOverride(pumpOn) {
      fetch('/api/override', {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json',
        },
        body: JSON.stringify({
          active: true,
          pumpState: pumpOn
        })
      })
      .then(response => response.json())
      .then(data => {
        fetchStatus(); // Status updaten
      })
      .catch(error => {
        console.error('Fout bij het activeren van override:', error);
      });
    }
    
    // Override annuleren
    function cancelOverride() {
      fetch('/api/override', {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json',
        },
        body: JSON.stringify({
          active: false
        })
      })
      .then(response => response.json())
      .then(data => {
        fetchStatus(); // Status updaten
      })
      .catch(error => {
        console.error('Fout bij het annuleren van override:', error);
      });
    }
  </script>
</body>
</html>
)rawliteral";

#endif // WEBUI_H