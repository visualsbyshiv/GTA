import { Pool } from 'pg';
import Redis from 'ioredis';
import dotenv from 'dotenv';

dotenv.config();

let useMockDb = false;

export interface User {
  id: number;
  username: string;
  password_hash: string;
}

export interface PlayerSave {
  user_id: number;
  health: number;
  armor: number;
  money: number;
  pos_x: number;
  pos_y: number;
  pos_z: number;
  rot_yaw: number;
  completed_missions: string[];
}

const mockUsers: User[] = [];
const mockPlayerSaves: PlayerSave[] = [];
let nextUserId = 1;

export const pgPool = new Pool({
  connectionString: process.env.DATABASE_URL,
  connectionTimeoutMillis: 1000, 
});

export let redis: Redis | null = null;

export async function initializeDatabase() {
  console.log('Testing database connections...');
  
  try {
    redis = new Redis(process.env.REDIS_URL || 'redis://localhost:6379', {
      maxRetriesPerRequest: 1,
      connectTimeout: 500,
      retryStrategy: () => null,
    });
    await redis.ping();
    console.log('Redis connected successfully.');
  } catch (err) {
    console.warn('⚠️ Redis is unreachable. Falling back to local cache.');
    if (redis) {
      redis.disconnect();
    }
    redis = null;
  }

  try {
    const client = await pgPool.connect();
    try {
      await client.query('SELECT 1');
      console.log('PostgreSQL connected successfully.');
      
      await client.query(`
        CREATE TABLE IF NOT EXISTS users (
          id SERIAL PRIMARY KEY,
          username VARCHAR(50) UNIQUE NOT NULL,
          password_hash VARCHAR(255) NOT NULL,
          created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
        );
      `);

      await client.query(`
        CREATE TABLE IF NOT EXISTS player_saves (
          id SERIAL PRIMARY KEY,
          user_id INTEGER UNIQUE REFERENCES users(id) ON DELETE CASCADE,
          health FLOAT DEFAULT 100.0,
          armor FLOAT DEFAULT 0.0,
          money INTEGER DEFAULT 5000,
          pos_x FLOAT DEFAULT 0.0,
          pos_y FLOAT DEFAULT 0.0,
          pos_z FLOAT DEFAULT 100.0,
          rot_yaw FLOAT DEFAULT 0.0,
          completed_missions VARCHAR(100)[] DEFAULT '{}',
          updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
        );
      `);
      console.log('PostgreSQL tables initialized.');
    } finally {
      client.release();
    }
  } catch (err) {
    console.warn('⚠️ PostgreSQL is unreachable. Falling back to local In-Memory Database.');
    useMockDb = true;
  }
}

export async function createUser(username: string, passwordHash: string): Promise<User> {
  if (useMockDb) {
    const exists = mockUsers.find(u => u.username === username);
    if (exists) {
      const err = new Error('Username already exists');
      (err as any).code = '23505';
      throw err;
    }
    const newUser: User = { id: nextUserId++, username, password_hash: passwordHash };
    mockUsers.push(newUser);
    
    mockPlayerSaves.push({
      user_id: newUser.id,
      health: 100.0,
      armor: 0.0,
      money: 5000,
      pos_x: 0.0,
      pos_y: 0.0,
      pos_z: 100.0,
      rot_yaw: 0.0,
      completed_missions: [],
    });

    return newUser;
  } else {
    const userResult = await pgPool.query(
      'INSERT INTO users (username, password_hash) VALUES ($1, $2) RETURNING id, username',
      [username, passwordHash]
    );
    const userId = userResult.rows[0].id;
    await pgPool.query('INSERT INTO player_saves (user_id) VALUES ($1)', [userId]);
    return userResult.rows[0];
  }
}

export async function findUser(username: string): Promise<User | null> {
  if (useMockDb) {
    const user = mockUsers.find(u => u.username === username);
    return user || null;
  } else {
    const result = await pgPool.query(
      'SELECT id, username, password_hash FROM users WHERE username = $1',
      [username]
    );
    return result.rows.length > 0 ? result.rows[0] : null;
  }
}

export async function getPlayerSave(userId: number): Promise<PlayerSave | null> {
  if (useMockDb) {
    const save = mockPlayerSaves.find(s => s.user_id === userId);
    return save || null;
  } else {
    const result = await pgPool.query(
      'SELECT health, armor, money, pos_x, pos_y, pos_z, rot_yaw, completed_missions FROM player_saves WHERE user_id = $1',
      [userId]
    );
    return result.rows.length > 0 ? result.rows[0] : null;
  }
}

export async function updatePlayerSave(
  userId: number,
  health?: number,
  armor?: number,
  money?: number,
  posX?: number,
  posY?: number,
  posZ?: number,
  rotYaw?: number,
  completedMission?: string
): Promise<PlayerSave> {
  if (useMockDb) {
    let save = mockPlayerSaves.find(s => s.user_id === userId);
    if (!save) {
      save = {
        user_id: userId,
        health: 100.0,
        armor: 0.0,
        money: 5000,
        pos_x: 0.0,
        pos_y: 0.0,
        pos_z: 100.0,
        rot_yaw: 0.0,
        completed_missions: [],
      };
      mockPlayerSaves.push(save);
    }

    if (health !== undefined && health !== null) save.health = health;
    if (armor !== undefined && armor !== null) save.armor = armor;
    if (money !== undefined && money !== null) save.money += money; 
    if (posX !== undefined && posX !== null) save.pos_x = posX;
    if (posY !== undefined && posY !== null) save.pos_y = posY;
    if (posZ !== undefined && posZ !== null) save.pos_z = posZ;
    if (rotYaw !== undefined && rotYaw !== null) save.rot_yaw = rotYaw;
    if (completedMission !== undefined && completedMission !== null) {
      if (!save.completed_missions.includes(completedMission)) {
        save.completed_missions.push(completedMission);
      }
    }

    return save;
  } else {
    const query = `
      INSERT INTO player_saves (user_id, health, armor, money, pos_x, pos_y, pos_z, rot_yaw, completed_missions)
      VALUES ($1, $2, $3, $4, $5, $6, $7, $8, CASE WHEN $9::varchar IS NOT NULL THEN ARRAY[$9::varchar] ELSE '{}'::varchar[] END)
      ON CONFLICT (user_id) DO UPDATE SET
        health = COALESCE(EXCLUDED.health, player_saves.health),
        armor = COALESCE(EXCLUDED.armor, player_saves.armor),
        money = CASE 
          WHEN EXCLUDED.money IS NOT NULL THEN player_saves.money + EXCLUDED.money 
          ELSE player_saves.money 
        END,
        pos_x = COALESCE(EXCLUDED.pos_x, player_saves.pos_x),
        pos_y = COALESCE(EXCLUDED.pos_y, player_saves.pos_y),
        pos_z = COALESCE(EXCLUDED.pos_z, player_saves.pos_z),
        rot_yaw = COALESCE(EXCLUDED.rot_yaw, player_saves.rot_yaw),
        completed_missions = CASE 
          WHEN $9::varchar IS NOT NULL AND NOT ($9::varchar = ANY(player_saves.completed_missions))
          THEN array_append(player_saves.completed_missions, $9::varchar)
          ELSE player_saves.completed_missions
        END,
        updated_at = CURRENT_TIMESTAMP
      RETURNING *;
    `;

    const values = [
      userId,
      health,
      armor,
      money,
      posX,
      posY,
      posZ,
      rotYaw,
      completedMission
    ];

    const updateResult = await pgPool.query(query, values);
    return updateResult.rows[0];
  }
}
