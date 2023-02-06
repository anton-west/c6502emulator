#ifndef LOGGING_H
#define LOGGING_H

#include <stdio.h>
#include "cpu.h"
#include "opcodes.h"

int print_ir(FILE *logptr, InstrInfo *ir, Processor *cpu) {
    switch (ir->addr_mode)
    {
    case ACC:
        fprintf(logptr, "%04X,%02X,,,,%s,A,,,,A:%02X,X:%02X,Y:%02X,P:%02X,SP:%02X,,CYC:,%u\n", ir->abs_addr, ir->byte_1, ir->opcode_mnemonic, cpu->acc, cpu->x_reg, cpu->y_reg, cpu->status_reg, cpu->sp, cpu->total_cycles);
        break;
    case ABS:
        fprintf(logptr, "%04X,%02X,%02X,%02X,,%s,$%02X%02X,{ABS},,,A:%02X,X:%02X,Y:%02X,P:%02X,SP:%02X,,CYC:,%u\n", ir->abs_addr, ir->byte_1, ir->byte_2, ir->byte_3, ir->opcode_mnemonic, ir->byte_3, ir->byte_2, cpu->acc, cpu->x_reg, cpu->y_reg, cpu->status_reg, cpu->sp, cpu->total_cycles);
        break;
    case ABX:
        fprintf(logptr, "%04X,%02X,%02X,%02X,,%s,$%02X%02X,{ABX},,,A:%02X,X:%02X,Y:%02X,P:%02X,SP:%02X,,CYC:,%u\n", ir->abs_addr, ir->byte_1, ir->byte_2, ir->byte_3, ir->opcode_mnemonic, ir->byte_3, ir->byte_2, cpu->acc, cpu->x_reg, cpu->y_reg, cpu->status_reg, cpu->sp, cpu->total_cycles);
        break;
    case ABY:
        fprintf(logptr, "%04X,%02X,%02X,%02X,,%s,$%02X%02X,{ABY},,,A:%02X,X:%02X,Y:%02X,P:%02X,SP:%02X,,CYC:,%u\n", ir->abs_addr, ir->byte_1, ir->byte_2, ir->byte_3, ir->opcode_mnemonic, ir->byte_3, ir->byte_2, cpu->acc, cpu->x_reg, cpu->y_reg, cpu->status_reg, cpu->sp, cpu->total_cycles);
        break;
    case IMM:
        fprintf(logptr, "%04X,%02X,%02X,,,%s,#$%02X,{IMM},,,A:%02X,X:%02X,Y:%02X,P:%02X,SP:%02X,,CYC:,%u\n", ir->abs_addr, ir->byte_1, ir->byte_2, ir->opcode_mnemonic, ir->byte_2, cpu->acc, cpu->x_reg, cpu->y_reg, cpu->status_reg, cpu->sp, cpu->total_cycles);
        break;
    case IMP:
        fprintf(logptr, "%04X,%02X,,,,%s,{IMP},,,,A:%02X,X:%02X,Y:%02X,P:%02X,SP:%02X,,CYC:,%u\n", ir->abs_addr, ir->byte_1, ir->opcode_mnemonic, cpu->acc, cpu->x_reg, cpu->y_reg, cpu->status_reg, cpu->sp, cpu->total_cycles);
        break;
    case IND:
        fprintf(logptr, "%04X,%02X,%02X,%02X,,%s,($%02X%02X),{IND},,,A:%02X,X:%02X,Y:%02X,P:%02X,SP:%02X,,CYC:,%u\n", ir->abs_addr, ir->byte_1, ir->byte_2, ir->byte_3, ir->opcode_mnemonic, ir->byte_3, ir->byte_2, cpu->acc, cpu->x_reg, cpu->y_reg, cpu->status_reg, cpu->sp, cpu->total_cycles);
        break;
    case IDX:
        fprintf(logptr, "%04X,%02X,%02X,,,%s,($%02X.X),{IDX},,,A:%02X,X:%02X,Y:%02X,P:%02X,SP:%02X,,CYC:,%u\n", ir->abs_addr, ir->byte_1, ir->byte_2, ir->opcode_mnemonic, ir->byte_2, cpu->acc, cpu->x_reg, cpu->y_reg, cpu->status_reg, cpu->sp, cpu->total_cycles);
        break;
    case IDY:
        fprintf(logptr, "%04X,%02X,%02X,,,%s,($%02X).Y,{IDY},,,A:%02X,X:%02X,Y:%02X,P:%02X,SP:%02X,,CYC:,%u\n", ir->abs_addr, ir->byte_1, ir->byte_2, ir->opcode_mnemonic, ir->byte_2, cpu->acc, cpu->x_reg, cpu->y_reg, cpu->status_reg, cpu->sp, cpu->total_cycles);
        break;
    case REL:
        //requires block and step wise declaration due to bits acting weird when written inline
        {   
            uint16_t adr = (uint16_t)ir->abs_addr & 0xFFFF;
            uint8_t byte2 = (uint16_t)ir->byte_2;
            uint16_t byte2_ext = ((byte2 & 0x80) > 0) ? (0xFF00 | byte2) : byte2;
            uint16_t result = adr + byte2_ext + 2;
            fprintf(logptr, "%04X,%02X,%02X,,,%s,$%02X,[$%04X],{REL},,A:%02X,X:%02X,Y:%02X,P:%02X,SP:%02X,,CYC:,%u\n", ir->abs_addr, ir->byte_1, ir->byte_2, ir->opcode_mnemonic, byte2, result, cpu->acc, cpu->x_reg, cpu->y_reg, cpu->status_reg, cpu->sp, cpu->total_cycles);
        }
        break;
    case ZPG:
        fprintf(logptr, "%04X,%02X,%02X,,,%s,$%02X,{ZPG},,,A:%02X,X:%02X,Y:%02X,P:%02X,SP:%02X,,CYC:,%u\n", ir->abs_addr, ir->byte_1, ir->byte_2, ir->opcode_mnemonic, ir->byte_2, cpu->acc, cpu->x_reg, cpu->y_reg, cpu->status_reg, cpu->sp, cpu->total_cycles);
        break;
    case ZPX:
        fprintf(logptr, "%04X,%02X,%02X,,,%s,$%02X,{ZPX},,,A:%02X,X:%02X,Y:%02X,P:%02X,SP:%02X,,CYC:,%u\n", ir->abs_addr, ir->byte_1, ir->byte_2, ir->opcode_mnemonic, ir->byte_2, cpu->acc, cpu->x_reg, cpu->y_reg, cpu->status_reg, cpu->sp, cpu->total_cycles);
        break;
    case ZPY:
        fprintf(logptr, "%04X,%02X,%02X,,,%s,$%02X,{ZPY},,,A:%02X,X:%02X,Y:%02X,P:%02X,SP:%02X,,CYC:,%u\n", ir->abs_addr, ir->byte_1, ir->byte_2, ir->opcode_mnemonic, ir->byte_2, cpu->acc, cpu->x_reg, cpu->y_reg, cpu->status_reg, cpu->sp, cpu->total_cycles);
        break;
    default:
        fprintf(logptr, "%04X,,,,,%s,{UDF}\n", ir->abs_addr, ir->opcode_mnemonic);
        break;
    }
    return 0;
}

#endif