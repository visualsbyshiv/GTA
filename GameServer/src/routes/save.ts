import { Router } from 'express';
import { getPlayerSave, updatePlayerSave } from '../db';

const router = Router();

router.get('/:userId', async (req: any, res: any) => {
  const { userId } = req.params;

  try {
    const saveState = await getPlayerSave(parseInt(userId, 10));

    if (!saveState) {
      return res.status(404).json({ error: 'Player save not found' });
    }

    res.json(saveState);
  } catch (error) {
    console.error(error);
    res.status(500).json({ error: 'Internal server error' });
  }
});

router.post('/:userId', async (req: any, res: any) => {
  const { userId } = req.params;
  const { health, armor, money, pos_x, pos_y, pos_z, rot_yaw, completed_mission } = req.body;

  try {
    const saveState = await updatePlayerSave(
      parseInt(userId, 10),
      health,
      armor,
      money,
      pos_x,
      pos_y,
      pos_z,
      rot_yaw,
      completed_mission
    );

    res.json({ message: 'Player state saved successfully', saveState });
  } catch (error) {
    console.error(error);
    res.status(500).json({ error: 'Internal server error' });
  }
});

export default router;
