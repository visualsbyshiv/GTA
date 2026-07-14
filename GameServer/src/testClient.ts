import http from 'http';

const API_BASE = 'http://localhost:3001';

function postJson(path: string, body: any): Promise<any> {
  return new Promise((resolve, reject) => {
    const data = JSON.stringify(body);
    const url = new URL(path, API_BASE);
    const req = http.request(
      url,
      {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json',
          'Content-Length': data.length,
        },
      },
      (res) => {
        let responseData = '';
        res.on('data', (chunk) => (responseData += chunk));
        res.on('end', () => {
          try {
            resolve(JSON.parse(responseData));
          } catch (e) {
            resolve(responseData);
          }
        });
      }
    );
    req.on('error', (err) => reject(err));
    req.write(data);
    req.end();
  });
}

function getJson(path: string): Promise<any> {
  return new Promise((resolve, reject) => {
    const url = new URL(path, API_BASE);
    http.get(url, (res) => {
      let responseData = '';
      res.on('data', (chunk) => (responseData += chunk));
      res.on('end', () => {
        try {
          resolve(JSON.parse(responseData));
        } catch (e) {
          resolve(responseData);
        }
      });
    }).on('error', (err) => reject(err));
  });
}

async function runTest() {
  console.log('--- GTA Save State API Tester ---');

  try {
    const username = `player_${Math.floor(Math.random() * 10000)}`;
    console.log(`\n[1] Registering player: ${username}...`);
    const regResult = await postJson('/auth/register', { username, password: 'secure_password' });
    console.log('Registration Response:', regResult);

    if (!regResult.user || !regResult.user.id) {
      console.error('Registration failed.');
      return;
    }
    const userId = regResult.user.id;

    console.log(`\n[2] Logging in as: ${username}...`);
    const loginResult = await postJson('/auth/login', { username, password: 'secure_password' });
    console.log('Login Response:', loginResult);

    console.log(`\n[3] Fetching initial save state for user ID ${userId}...`);
    const initialState = await getJson(`/save/${userId}`);
    console.log('Initial Save State:', initialState);

    console.log('\n[4] Simulating player movement to new coordinates...');
    const moveResult = await postJson(`/save/${userId}`, {
      pos_x: 1543.2,
      pos_y: -890.4,
      pos_z: 32.5,
      rot_yaw: 180.0,
      health: 90.0,
      armor: 50.0,
    });
    console.log('Movement Save Response:', moveResult);

    console.log('\n[5] Completing mission "MISSION_JEWEL_STORE" (Reward: $25000)...');
    const missionResult = await postJson(`/save/${userId}`, {
      money: 25000,
      completed_mission: 'MISSION_JEWEL_STORE',
    });
    console.log('Mission Complete Response:', missionResult);

    console.log('\n[6] Completing mission "MISSION_THE_HEIST" (Reward: $50000)...');
    const heistResult = await postJson(`/save/${userId}`, {
      money: 50000,
      completed_mission: 'MISSION_THE_HEIST',
    });
    console.log('Heist Complete Response:', heistResult);

    console.log(`\n[7] Fetching final save state for user ID ${userId} to check increments & arrays...`);
    const finalState = await getJson(`/save/${userId}`);
    console.log('Final Save State:', finalState);

  } catch (error) {
    console.error('Test execution failed:', error);
  }
}

runTest();
