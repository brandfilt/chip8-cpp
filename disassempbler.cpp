#include <iostream>
#include <iomanip>
#include <cstdint>

void disassemble_chip8(uint8_t *codebuffer, int pc) {
    uint8_t *code = &codebuffer[pc];
    uint8_t firstword = (code[0] >> 4);

    std::cout << std::hex << std::setfill('0') << std::setw(4) 
        << pc << " " 
        << std::setw(2) << static_cast<int>(code[0]) << " " 
        << std::setw(2) << static_cast<int>(code[1]) << " ";

    switch(firstword) {
        case 0x00: 
            if(code[1] == 0x0E) {
                //CLS
                //Clear the screen
                std::cout << "CLS";
            } else if(code[1] == 0xEE) {
                //RTS
                //Return from subroutine
                std::cout << "RTS";
            }
        break;
        case 0x01: {
            //JUMP $NNN
            //Jump to address NNN
            uint8_t addresshi = code[0] & 0x0F;
            std::cout << "JUMP #$" 
                << std::setw(1) << static_cast<int>(addresshi)
                << std::setw(2) << static_cast<int>(code[1]);
        }
        break;
        case 0x02: {
            //CALL $NNN
            //Jump to subroutine at NNN
            uint8_t addresshi = code[0] & 0x0F;
            std::cout << "CALL #$" 
                << std::setw(1) << static_cast<int>(addresshi)
                << std::setw(2) << static_cast<int>(code[1]);
        }
        break;
        case 0x03: {
            //SKIP.EQ VX, #$NN
            //Skip the next instruction if VX equals NN
            uint8_t addresshi = code[0] & 0x0F;
            std::cout << "SKIP.EQ V"
                << std::setw(1) << static_cast<int>(addresshi)
                << ", #$"
                << std::setw(2) << static_cast<int>(code[1]);
        }
        break;
        case 0x04: {
            //SKIP.NQ VX, #$NN
            //Skip the next instruction if VX doesn't equal NN
            uint8_t addresshi = code[0] & 0x0F;
            std::cout << "SKIP.NQ V"
                << std::setw(1) << static_cast<int>(addresshi)
                << ", #$"
                << std::setw(2) << static_cast<int>(code[1]);
        }
        break;
        case 0x05: {
            //SKIP.EQ VX, VY
            //Skip the next instruction if VX equals VY
            uint8_t firstaddress = code[0] & 0x0F;
            uint8_t secondaddress = code[1] & 0xF0;
            std::cout << "SKIP.EQ V"
                << std::setw(1) << static_cast<int>(firstaddress)
                << ", V"
                << std::setw(1) << static_cast<int>(secondaddress);
        }
        break;
        case 0x06: {
            //MVI VX,NN
            //Sets register VX to NN
            uint8_t reg = code[0] & 0x0F;
            std::cout << "MVI V" 
                << std::setw(1) << static_cast<int>(reg) << ",#$" 
                << std::setw(2) << static_cast<int>(code[1]);
        }
        break;
        case 0x07: {
            //ADD VX, #$NN
            //Add NN to value in register VX
            uint8_t reg = code[0] & 0x0F;
            std::cout << "ADD V" 
                << std::setw(1) << static_cast<int>(reg) << ",#$" 
                << std::setw(2) << static_cast<int>(code[1]);
        }
        break;
        case 0x08: { 
            //8XYN have 9 different operations using two registers
            uint8_t operation = code[1] & 0x0F;
            uint8_t regx = code[0] & 0x0F;
            uint8_t regy = code[1] & 0xF0;
            switch(operation) {
                case 0x00: {
                    //MOV VX, VY
                    //Set VX to the value of VY
                    std::cout << "MOV V"
                        << std::setw(1) << static_cast<int>(regx)
                        << ", V"
                        << std::setw(1) << static_cast<int>(regy);
                }
                break;
                case 0x01: {
                    //OR VX, VY
                    //Set VX to VX OR VY 
                    std::cout << "OR V"
                        << std::setw(1) << static_cast<int>(regx)
                        << ", V"
                        << std::setw(1) << static_cast<int>(regy);
                }
                break;
                case 0x02: {
                    //AND VX, VY
                    //Set VX to VX AND VY 
                    std::cout << "AND V"
                        << std::setw(1) << static_cast<int>(regx)
                        << ", V"
                        << std::setw(1) << static_cast<int>(regy);
                }
                break;
                case 0x03: {
                    //XOR VX, VY
                    //Set VX to VX XOR VY 
                    std::cout << "XOR V"
                        << std::setw(1) << static_cast<int>(regx)
                        << ", V"
                        << std::setw(1) << static_cast<int>(regy);
                }
                break;
                case 0x04: {
                    //ADD. VX, VY
                    //Add VX to VY, VF will be set to 1 if there's a carry and to 0 if there's not
                    std::cout << "ADD. V"
                        << std::setw(1) << static_cast<int>(regx)
                        << ", V"
                        << std::setw(1) << static_cast<int>(regy);
                }
                break;
                case 0x05: {
                    //SUB. VX, VY
                    //Substract VY from VX. VF is set to 0 if there's a borrow, and to 1 if there's not.
                    std::cout << "SUB. V"
                        << std::setw(1) << static_cast<int>(regx)
                        << ", V"
                        << std::setw(1) << static_cast<int>(regy);
                }
                break;
                case 0x06: {
                    //SHR. VX
                    //Shift VX right by one. VF is set to the value of th least significant bit of VX before the shift.
                    std::cout << "SHR. V"
                        << std::setw(1) << static_cast<int>(regx);
                }
                break;
            }
        }
        break;
        case 0x09: {
            //SKIP.NQ VX, VY
            //Skip the next instruction if VX doesn't equal VY
            uint8_t firstaddress = code[0] & 0x0F;
            uint8_t secondaddress = code[1] & 0xF0;
            std::cout << "SKIP.NQ V"
                << std::setw(1) << static_cast<int>(firstaddress)
                << ", V"
                << std::setw(1) << static_cast<int>(secondaddress);
        }
        case 0x0a: {
            //MVI INNN
            //Sets I to the adress of NNN
            uint8_t addresshi = code[0] & 0x0F;
            std::cout << "MVI I,#$" 
                << std::setw(1) << static_cast<int>(addresshi)
                << std::setw(2) << static_cast<int>(code[1]);
            
        }
        break;
    }
}

int main(int argc, char **argv) {
    FILE *f = fopen(argv[1], "rb");

    if(f == NULL) {
        printf("error: Couldn't open %s\n", argv[1]);
        exit(1);
    }

    fseek(f, 0L, SEEK_END);
    int fsize = ftell(f);
    fseek(f, 0L, SEEK_SET);

    unsigned char *buffer = (unsigned char*)malloc(fsize+0x200);
    fread(buffer+0x200, fsize, 1, f);
    fclose(f);

    int pc = 0x200;
    while(pc < (fsize+0x200)) {
        disassemble_chip8(buffer, pc);
        pc += 2;
        printf("\n");
    }

    return 0;
}
