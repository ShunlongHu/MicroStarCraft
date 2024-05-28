mod strategy;

use std::cmp::PartialEq;
use std::error::Error;
use std::time::Duration;
use std::sync::Mutex;
use std::sync::Arc;

use message::rts_client::RtsClient;
use message::PlayerRequest;
use clap::Parser;
use clap::ValueEnum;

use tonic::transport::Channel;
use tonic::Request;
use tokio::time;

use strategy::strategy::act;
use strategy::strategy::GameState;
use strategy::strategy::GameObj;
use crate::message::Action;
use crate::Side::PlayerA;

pub mod message {
    tonic::include_proto!("message");
}

#[derive(Parser)]
#[command(version, author, about, long_about = None)]
struct Cli {
    /// connection_string, e.g.: http://127.0.0.1:8888
    connection_string: String,

    /// Player side: player-a|player-b
    side: Side,
}

#[derive(Copy, Clone, PartialEq, Eq, PartialOrd, Ord, ValueEnum, Debug)]
enum Side {
    /// connect as PlayerA
    PlayerA,
    /// connect as PlayerB
    PlayerB,
}

async fn main_loop(client: &mut RtsClient<Channel>, side: i8) -> Result<(), Box<dyn Error>> {
    let game_state = Arc::new(Mutex::new(GameState{
        obj_vec: vec![], resource: [0;2],building_cnt: [0;2],w: 0,h: 0,time: 0,obj_cnt: 0,
    }));
    let game_state_clone = game_state.clone();
    let outbound = async_stream::stream! {
        yield PlayerRequest{command: 0,role: if side == -1 {i32::from(message::Role::PlayerA)} else {i32::from(message::Role::PlayerB)}, actions: vec![Action{
id: 0,action: 0,produce_type: 0,target_x: 0,target_y: 0,}],};
        let mut interval = time::interval(Duration::from_micros(100));
        let mut last_game_state = GameState{obj_vec: vec![], resource: [0;2],building_cnt: [0;2],w: 0,h: 0,time: 0,obj_cnt: 0,};
        let mut cur_game_state = last_game_state.clone();
        loop {
            {
                let gs = game_state_clone.lock().unwrap().clone();
                cur_game_state = gs.clone();
                // cur_game_state.obj_size = gs.obj_size;
                // cur_game_state.obj_arr = gs.obj_arr.clone();
                // cur_game_state.building_cnt = gs.building_cnt.clone();
                // cur_game_state.resource = gs.resource.clone();
                // cur_game_state.w = gs.w;
                // cur_game_state.h = gs.h;
                // cur_game_state.time = gs.time;
                // cur_game_state.obj_cnt = gs.obj_cnt;
            }

            if last_game_state != cur_game_state {
                last_game_state = cur_game_state.clone();
                let action = act(last_game_state.clone(), side);
                let mut request = PlayerRequest{
                    command: 0,
                    role: if side == -1 {i32::from(message::Role::PlayerA)} else {i32::from(message::Role::PlayerB)},
                    actions: vec![]
                };
                for (idx, act) in action {
                    request.actions.push(Action{id: idx, action: act.action, produce_type: act.produce_type, target_x: act.target.x, target_y: act.target.y});
                }
                yield request;
            }
            interval.tick().await;
        }
    };

    let response = client.connect_player(Request::new(outbound)).await?;
    let mut inbound = response.into_inner();

    while let Some(message) = inbound.message().await? {
        println!("{:?}", message.data.len());
        let mut data = game_state.lock().unwrap();
        let mut idx = 0;

        let obj_num = i64::from_le_bytes(message.data[idx..idx+8].to_vec().try_into().unwrap());
        data.obj_vec.reserve(obj_num as usize);
        idx += 8;
        for _ in 0..obj_num {
            let obj_idx = i32::from_le_bytes(message.data[idx..idx+4].to_vec().try_into().unwrap());
            idx += 4;
            let mut obj: GameObj = GameObj {..Default::default()};
            obj.obj_type = i32::from_le_bytes(message.data[idx..idx+4].to_vec().try_into().unwrap());
            idx += 4;
            obj.coord.y = i32::from_le_bytes(message.data[idx..idx+4].to_vec().try_into().unwrap());
            idx += 4;
            obj.coord.x = i32::from_le_bytes(message.data[idx..idx+4].to_vec().try_into().unwrap());
            idx += 4;
            obj.hit_point = i16::from_le_bytes(message.data[idx..idx+2].to_vec().try_into().unwrap());
            idx += 2;
            obj.resource = i16::from_le_bytes(message.data[idx..idx+2].to_vec().try_into().unwrap());
            idx += 2;
            obj.owner = i8::from_le_bytes(message.data[idx..idx+1].to_vec().try_into().unwrap());
            idx += 1;
            idx += 3;
            obj.current_action = i32::from_le_bytes(message.data[idx..idx+4].to_vec().try_into().unwrap());
            idx += 4;
            obj.action_target.y = i32::from_le_bytes(message.data[idx..idx+4].to_vec().try_into().unwrap());
            idx += 4;
            obj.action_target.x = i32::from_le_bytes(message.data[idx..idx+4].to_vec().try_into().unwrap());
            idx += 4;
            obj.action_progress = i16::from_le_bytes(message.data[idx..idx+2].to_vec().try_into().unwrap());
            idx += 2;
            obj.action_total_progress = i16::from_le_bytes(message.data[idx..idx+2].to_vec().try_into().unwrap());
            idx += 2;
            obj.attack_cd = i16::from_le_bytes(message.data[idx..idx+2].to_vec().try_into().unwrap());
            idx += 2;
            idx += 2;
            obj.produce_type = i32::from_le_bytes(message.data[idx..idx+4].to_vec().try_into().unwrap());
            idx += 4;
            obj.attack_range = i16::from_le_bytes(message.data[idx..idx+2].to_vec().try_into().unwrap());
            idx += 2;
            obj.attack_interval = i16::from_le_bytes(message.data[idx..idx+2].to_vec().try_into().unwrap());
            idx += 2;
            obj.attack_point = i16::from_le_bytes(message.data[idx..idx+2].to_vec().try_into().unwrap());
            idx += 2;
            obj.action_mask.can_move = message.data[idx] > 0;
            idx += 1;
            obj.action_mask.can_attack = message.data[idx] > 0;
            idx += 1;
            obj.action_mask.can_gather = message.data[idx] > 0;
            idx += 1;
            obj.action_mask.can_be_stored = message.data[idx] > 0;
            idx += 1;
            obj.action_mask.can_be_attacked = message.data[idx] > 0;
            idx += 1;
            obj.action_mask.can_be_gathered = message.data[idx] > 0;
            idx += 1;
            data.obj_vec.push(obj);
        }
        data.resource[0] = i32::from_le_bytes(message.data[idx..idx+4].to_vec().try_into().unwrap());
        idx += 4;
        data.resource[1] = i32::from_le_bytes(message.data[idx..idx+4].to_vec().try_into().unwrap());
        idx += 4;
        data.building_cnt[0] = i32::from_le_bytes(message.data[idx..idx+4].to_vec().try_into().unwrap());
        idx += 4;
        data.building_cnt[1] = i32::from_le_bytes(message.data[idx..idx+4].to_vec().try_into().unwrap());
        idx += 4;
        data.w = i32::from_le_bytes(message.data[idx..idx+4].to_vec().try_into().unwrap());
        idx += 4;
        data.h = i32::from_le_bytes(message.data[idx..idx+4].to_vec().try_into().unwrap());
        idx += 4;
        data.time = i32::from_le_bytes(message.data[idx..idx+4].to_vec().try_into().unwrap());
        idx += 4;
        data.obj_cnt = i32::from_le_bytes(message.data[idx..idx+4].to_vec().try_into().unwrap());
    }

    Ok(())
}
#[tokio::main]
async fn main() -> Result<(), Box<dyn Error>> {
    let cli = Cli::parse();
    println!("connection_string: {}", cli.connection_string);
    println!("side: {:?}", cli.side);

    let mut client = RtsClient::connect(cli.connection_string).await?;
    main_loop(&mut client, if cli.side == PlayerA {-1} else {1}).await?;
    Ok(())
}