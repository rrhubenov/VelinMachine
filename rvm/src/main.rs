use std::fs::{metadata, File};
use std::io::Read;
use std::ops::{BitAndAssign, BitOrAssign};
use std::process;
use std::vec::Vec;
use std::{cmp, env};

#[derive(Debug)]
enum Instruction {
    // Memory
    Loadi, // Load immidiate
    Load,
    Store,
    // Algebraic ops
    Add,
    Sub,
    Mul,
    // Logical ops
    And,
    Or,
    Not,
    // Conditionals
    Cmp,
    // Jumps
    Jne,
    Jeq,
    // Etc
    Hlt,
    Prnt,
    Noop,
}

const REGISTER_COUNT: usize = 16;
const RAM_SIZE: usize = 0xffff;
struct Cpu {
    ram: [u32; RAM_SIZE],
    regs: [u32; REGISTER_COUNT],
    pc: u16,
    ir: [u8; 4],
    psw: u32,
    instruction: Instruction,
    debug: bool,
}

impl Cpu {

    pub fn new(boot: [u32; RAM_SIZE], debug: bool) -> Self {
        Self {
            ram: boot,
            regs: [0; 16],
            pc: 0,
            ir: [0, 0, 0, 0],
            psw: 0,
            instruction: Instruction::Noop,
            debug,
        }
    }

    fn set_zero_flag(&mut self) {
        self.psw.bitor_assign(1);
    }
    fn unset_zero_flag(&mut self) {
        self.psw.bitand_assign(!(1));
    }

    fn set_negative_flag(&mut self) {
        self.psw.bitor_assign(2);
    }
    fn unset_negative_flag(&mut self) {
        self.psw.bitand_assign(!(2));
    }

    fn zero_flag(&self) -> bool {
        self.psw & 1 == 1
    }

    fn negative_flag(&self) -> bool {
        self.psw & 2 == 2
    }

    fn fetch(&mut self) {
        self.ir = self.ram[self.pc as usize].to_ne_bytes();
    }

    fn print_state(&self) {
        println!("Current instruction: {:?}", self.instruction);
        println!("IR: {:x?}", self.ir);
        println!("PSW: {:032b}", self.psw);
        for reg in 0..REGISTER_COUNT {
            println!("reg {}: {:#010x}", reg, self.regs[reg]);
        }
    }

    pub fn run(&mut self) {
        loop {
            self.fetch();
            self.decode();
            if self.debug {
                self.print_state();
            }
            self.execute();
            self.pc += 1;
        }
    }

    fn execute(&mut self) {
        match self.instruction {
            Instruction::Hlt => std::process::exit(0x0),
            Instruction::Noop => {}
            Instruction::Loadi => {
                let destination_reg = self.ir[1];
                let value: u16 = u16::from_le_bytes([self.ir[2], self.ir[3]]);
                self.regs[destination_reg as usize] = value as u32;
            }
            Instruction::Prnt => {
                let source_reg = self.ir[1];
                println!("{}", self.regs[source_reg as usize]);
            }
            Instruction::Not => {
                let destination_reg = self.ir[1];
                // Bitwise not in Rust
                self.regs[destination_reg as usize] = !self.regs[destination_reg as usize];
            }
            Instruction::Add => {
                let destination_reg = self.ir[1];
                let r1 = self.ir[2];
                let r2 = self.ir[3];
                self.regs[destination_reg as usize] =
                    self.regs[r1 as usize] + self.regs[r2 as usize];
            }
            Instruction::Sub => {
                let destination_reg = self.ir[1];
                let r1 = self.ir[2];
                let r2 = self.ir[3];
                self.regs[destination_reg as usize] =
                    self.regs[r1 as usize] - self.regs[r2 as usize];
            }
            Instruction::Mul => {
                let destination_reg = self.ir[1];
                let r1 = self.ir[2];
                let r2 = self.ir[3];
                self.regs[destination_reg as usize] =
                    self.regs[r1 as usize] * self.regs[r2 as usize];
            }
            Instruction::And => {
                let destination_reg = self.ir[1];
                let r1 = self.ir[2];
                let r2 = self.ir[3];
                self.regs[destination_reg as usize] =
                    self.regs[r1 as usize] & self.regs[r2 as usize];
            }
            Instruction::Or => {
                let destination_reg = self.ir[1];
                let r1 = self.ir[2];
                let r2 = self.ir[3];
                self.regs[destination_reg as usize] =
                    self.regs[r1 as usize] | self.regs[r2 as usize];
            }
            Instruction::Cmp => {
                let r1 = self.ir[1];
                let r2 = self.ir[2];

                match self.regs[r1 as usize].cmp(&self.regs[r2 as usize]) {
                    cmp::Ordering::Equal => {
                        self.set_zero_flag();
                        self.unset_negative_flag()
                    }
                    cmp::Ordering::Less => {
                        self.unset_zero_flag();
                        self.set_negative_flag();
                    }
                    cmp::Ordering::Greater => {
                        self.unset_zero_flag();
                        self.unset_negative_flag();
                    }
                }
            }
            Instruction::Load => {
                let reg = self.ir[1];
                let addr = u16::from_le_bytes([self.ir[2], self.ir[3]]);

                self.regs[reg as usize] = self.ram[addr as usize];
            }
            Instruction::Store => {
                let reg = self.ir[1];
                let addr = u16::from_le_bytes([self.ir[2], self.ir[3]]);

                self.ram[addr as usize] = self.regs[reg as usize];
            }
            Instruction::Jeq => {
                if self.zero_flag() {
                    let addr = u16::from_ne_bytes([self.ir[1], self.ir[2]]);
                    self.pc = addr - 1;
                }
            } 
            Instruction::Jne => {
                if !self.zero_flag() {
                    let addr = u16::from_ne_bytes([self.ir[1], self.ir[2]]);
                    self.pc = addr - 1;
                }
            }
        }
    }


    // TODO: Might be e good idea to export to some table/map of type (Num => Instruction)
    fn decode(&mut self) {
        let opcode = self.ir[0];
        self.instruction = match opcode {
            0x00 => Instruction::Noop,
            0x01 => Instruction::Loadi,
            0x02 => Instruction::Add,
            0x03 => Instruction::Prnt,
            0x04 => Instruction::And,
            0x05 => Instruction::Or,
            0x06 => Instruction::Not,
            0x07 => Instruction::Jne,
            0x08 => Instruction::Sub,
            0x09 => Instruction::Mul,
            0x0a => Instruction::Cmp,
            0x0b => Instruction::Jeq,
            0x0c => Instruction::Load,
            0x0d => Instruction::Store,
            0xff => Instruction::Hlt,
            unknown => panic!("Unknown opcode: {:2x}, at pc: {:8x}", unknown, self.pc),
        }
    }
}

fn main() {
    let args: Vec<String> = env::args().collect();
    let argc = args.len();
    let mut debug_flag = false;

    if argc == 1 {
        eprintln!("Error: expected 1 or 2 arguments, but got {}", argc - 1);
        process::exit(1);
    }

    if argc == 3 && args[2] == "-d" {
        if args[2] == "-d" {
            debug_flag = true;
        } else {
            eprintln!("Error: got a second argument but it wasn't '-d'");
        }
    }

    if argc > 3 {
        panic!("Too many arguments!");
    }

    let metadata = metadata(&args[1]).expect("Could not read file metadata");

    if metadata.len() > RAM_SIZE as u64 {
        panic!(
            "File size is greater than ram size: {} > {}",
            metadata.len(),
            RAM_SIZE
        );
    }

    let mut file = File::open(&args[1]).expect("Failed to open file");
    let boot: [u32; RAM_SIZE];

    unsafe {
        let mut buffer: [u8; RAM_SIZE * 4] = [0; RAM_SIZE * 4];
        let _read_count = file.read(&mut buffer[..]).expect("Could not read file");
        boot = std::mem::transmute(buffer);
    }

    let mut cpu = Cpu::new(boot, debug_flag);
    cpu.run();
}
