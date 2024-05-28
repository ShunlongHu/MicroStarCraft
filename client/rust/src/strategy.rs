    pub mod strategy {
    const TERRAIN : i32=0;
    const MINERAL : i32=1;
    const BASE : i32=2;
    const BARRACK : i32=3;
    const WORKER : i32=4;
    const LIGHT : i32=5;
    const HEAVY : i32=6;
    const RANGED : i32=7;

    const NOOP : i32=0;
    const MOVE : i32=1;
    const GATHER : i32=2;
    const RETURN : i32=3;
    const PRODUCE : i32=4;
    const ATTACK : i32=5;

    const PLAYER_A : i8=-1;
    const PLAYER_B : i8=1;

    #[derive(Copy, Clone, Ord, PartialOrd, Eq, PartialEq, Default)]
    pub struct Coord {
        pub y: i32,
        pub x: i32
    }
    #[derive(Copy, Clone, Ord, PartialOrd, Eq, PartialEq, Default)]
    pub struct ActionMask {
        pub can_move: bool,
        pub can_attack: bool,
        pub can_gather: bool,
        pub can_be_stored: bool,
        pub can_be_attacked: bool,
        pub can_be_gathered: bool,
    }

    #[derive(Copy, Clone, Ord, PartialOrd, Eq, PartialEq, Default)]
    pub struct GameObj {
        pub idx: i32,
        pub obj_type: i32,
        pub coord: Coord,
        pub hit_point: i16,
        pub resource: i16,
        pub owner: i8,
        pub current_action: i32,
        pub action_target: Coord,
        pub action_progress: i16,
        pub action_total_progress: i16,
        pub attack_cd: i16,
        pub produce_type: i32,
        pub attack_range: i16,
        pub attack_interval: i16,
        pub attack_point: i16,
        pub action_mask: ActionMask
    }

    #[derive(Clone, Ord, PartialOrd, Eq, PartialEq)]
    pub struct GameState {
        pub obj_vec: Vec<GameObj>,
        pub resource: [i32; 2],
        pub building_cnt: [i32; 2],
        pub w: i32,
        pub h: i32,
        pub time: i32,
        pub obj_cnt: i32,
    }

    #[derive(Copy, Clone, Ord, PartialOrd, Eq, PartialEq, Default)]
    pub struct Action {
        pub action: i32,
        pub produce_type: i32,
        pub target: Coord,
    }

    use std::collections::HashMap;
    use rand::Rng;

    pub fn act(game_state: GameState, side: i8) -> HashMap<i32, Action> {
        let mut ret_val: HashMap<i32, Action> = HashMap::new();
        for obj in game_state.obj_vec {
            if obj.owner != side {
                continue;
            }
            let action: Action = Action {
                action: rand::thread_rng().gen_range(0..ATTACK + 1),
                produce_type: rand::thread_rng().gen_range(0..RANGED + 1),
                target: Coord {y: rand::thread_rng().gen_range(0..game_state.h), x:rand::thread_rng().gen_range(0..game_state.w)},
            };
            ret_val.insert(obj.idx, action);
        }
        ret_val
    }
}
