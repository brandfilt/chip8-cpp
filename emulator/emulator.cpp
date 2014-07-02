#include <iostream>
#include <iomanip>
#include <stdint.h>
#include <fstream>

class chip8 {
public:
    chip8() {
        this->memory =  new unsigned char[1024*4+0x200]; //4K memory reserved
        this->SP = 0xfa0;
        this->PC = 0x200; //Programs are loaded at 0x200
    }

    void load_rom(char *filename) {
        std::ifstream rom(filename, std::ios::in | std::ios::binary | std::ios::ate);

        if(!rom.is_open()) {
            std::cout << "Couldn't open file!" << std::endl;
        }

        int size = rom.tellg();
        
        //Load rom to memory at 0x200
        rom.seekg(0, std::ios::beg);
        rom.read((char*)this->memory+0x200, size);
        rom.close();
    }

    void emulate() {
        //Fetch next instruction
        uint8_t *op = &this->memory[this->PC];
        
        int highnib = (*op & 0xf0) >> 4;
        switch(highnib) {
            case 0x00:
            break;
            case 0x01: {
                //JUMP $NNN
                //Jump to NNN
                uint16_t target = ((code[0] & 0xf) << 8) | code[1];
                this->PC = target;
            }
            break;
            case 0x02: {
                //CALL $NNN
                //Call subroutine at NNN

                //Advance stack pointer
                this->SP = -2;

                //Store next instructions address to memory pointed by the stack pointer
                memory[this->SP] = ((this->PC+2) & 0xff00) >> 8;
                memory[this->SP+1] = ((this->PC+2) & 0x00ff);
                
                //Jump to subroutines address NNN
                uint16_t target = ((code[0] & 0xf) << 8) | code[1];
                this->PC = target;
            }
            break;
            case 0x03: {
                uint8_t reg = code[0] & 0xf;
                if(this->V[reg] == code[1])
                    this-PC += 2;
                this->PC += 2;
            }
            break;
            case 0x06: {
                uint8_t reg = code[0] & 0xf;
                this->V[reg] = code[1]
                this->PC += 2;
            }
            break;
            case 0x0a: {
                this-I = ((code[0] & 0xf) << 8) | code[1];
                this->PC += 2;
            }
            break;
        }
    }
    
private:
    uint8_t     V[16];  //Registers 0-F
    uint16_t    I;      //Index register
    uint16_t    SP;     //Stack pointer
    uint16_t    PC;     //Program counter
    uint8_t     delay;  //Delay timer
    uint8_t     sound;  //Sound timer
    uint8_t     *memory;
    uint8_t     *screen;    //Same as memory[0xF00]
};


int main(int argc, char **argv) {
    chip8 vm;

    vm.load_rom(argv[1]);
    return 0;
}
