import express from 'express';
import cors from 'cors';
import dotenv from 'dotenv';
import authRoutes from './routes/auth';
import saveRoutes from './routes/save';
import { initializeDatabase } from './db';

dotenv.config();

const app = express();
const PORT = process.env.PORT || 3000;

app.use(cors());
app.use(express.json());

app.use('/auth', authRoutes);
app.use('/save', saveRoutes);

app.get('/health', (req: any, res: any) => {
  res.json({ status: 'healthy', timestamp: new Date() });
});

async function main() {
  console.log('Booting game state server...');
  await initializeDatabase();
  
  app.listen(PORT, () => {
    console.log(`GTA Game Server listening on port ${PORT}`);
  });
}

main().catch((err) => {
  console.error('Fatal error starting server:', err);
});
