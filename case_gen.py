register = ["B", "C", "D", "E", "H", "L", "@", "A"]
register_pair = ["BC", "DE", "HL", "SP"]
condition = ["NZ", "Z", "NC", "C", "PO", "PE", "P", "M"]

REGS = 8
RPS = 4
CONDS = 8

def data_transfer_group():
    print("/* DATA TRANSFER GROUP */")
    print("")

    for ddd in range(0, REGS):
        if ddd == 6:
            continue
        for sss in range(0, REGS):
            if sss == 6:
                continue
            case_bin = '01{0:03b}{1:03b}'.format(ddd, sss)
            case_hex = int(case_bin, 2)
            print('case 0x{:03x}: /* MOV {}, {} */'.format(case_hex, register[ddd], register[sss]))
    print("{ \n} break;\n")

    for ddd in range(0, REGS):
        if ddd == 6:
            continue
        case_bin = '01{0:03b}110'.format(ddd)
        case_hex = int(case_bin, 2)
        print('case 0x{:03x}: /* MOV {}, M */'.format(case_hex, register[ddd]))
    print("{ \n} break;\n")

    for sss in range(0, REGS):
        if sss == 6:
            continue
        case_bin = '01110{0:03b}'.format(sss)
        case_hex = int(case_bin, 2)
        print('case 0x{:03x}: /* MOV M, {} */'.format(case_hex, register[sss]))
    print("{ \n} break;\n")

    for ddd in range(0, REGS):
        if ddd == 6:
            continue
        case_bin = '00{0:03b}110'.format(ddd)
        case_hex = int(case_bin, 2)
        print('case 0x{:03x}: /* MVI {}, data */'.format(case_hex, register[ddd]))
    print("{ \n} break;\n")

    case_bin = '00110110'
    case_hex = int(case_bin, 2)
    print('case 0x{:03x}: /* MVI M, data */'.format(case_hex))
    print("{ \n} break;\n")


    for rp in range(0, RPS):
        case_bin = '00{0:02b}0001'.format(rp)
        case_hex = int(case_bin, 2)
        print('case 0x{:03x}: /* LXI {}, data 16 */'.format(case_hex, register_pair[rp]))
    print("{ \n} break;\n")

    case_bin = '00111010'
    case_hex = int(case_bin, 2)
    print('case 0x{:03x}: /* LDA addr */'.format(case_hex))
    print("{ \n} break;\n")

    case_bin = '00110010'
    case_hex = int(case_bin, 2)
    print('case 0x{:03x}: /* STA addr */'.format(case_hex))
    print("{ \n} break;\n")

    case_bin = '00101010'
    case_hex = int(case_bin, 2)
    print('case 0x{:03x}: /* LHLD addr */'.format(case_hex))
    print("{ \n} break;\n")

    case_bin = '00100010'
    case_hex = int(case_bin, 2)
    print('case 0x{:03x}: /* SHLD addr */'.format(case_hex))
    print("{ \n} break;\n")

    for rp in range(0, RPS - 2):
        case_bin = '00{0:02b}1010'.format(rp)
        case_hex = int(case_bin, 2)
        print('case 0x{:03x}: /* LDAX {} */'.format(case_hex, register_pair[rp]))
    print("{ \n} break;\n")

    for rp in range(0, RPS - 2):
        case_bin = '00{0:02b}0010'.format(rp)
        case_hex = int(case_bin, 2)
        print('case 0x{:03x}: /* STAX {} */'.format(case_hex, register_pair[rp]))
    print("{ \n} break;\n")

    case_bin = '11101011'
    case_hex = int(case_bin, 2)
    print('case 0x{:03x}: /* XCHG */'.format(case_hex))
    print("{ \n} break;\n")
    print("")

def arithmetic_group():
    print("/* ARITHMETIC GROUP */")
    print("")

    for sss in range(0, REGS):
        if sss == 6:
            continue
        case_bin = '10000{0:03b}'.format(sss)
        case_hex = int(case_bin, 2)
        print('case 0x{:03x}: /* ADD {} */'.format(case_hex, register[sss]))
    print("{ \n} break;\n")

    case_bin = '10000110'
    case_hex = int(case_bin, 2)
    print('case 0x{:03x}: /* ADD M */'.format(case_hex))
    print("{ \n} break;\n")

    case_bin = '11000110'
    case_hex = int(case_bin, 2)
    print('case 0x{:03x}: /* ADI data */'.format(case_hex))
    print("{ \n} break;\n")

    for sss in range(0, REGS):
        if sss == 6:
            continue
        case_bin = '10001{0:03b}'.format(sss)
        case_hex = int(case_bin, 2)
        print('case 0x{:03x}: /* ADC {} */'.format(case_hex, register[sss]))
    print("{ \n} break;\n")

    case_bin = '10001110'
    case_hex = int(case_bin, 2)
    print('case 0x{:03x}: /* ADC M */'.format(case_hex))
    print("{ \n} break;\n")

    case_bin = '11001110'
    case_hex = int(case_bin, 2)
    print('case 0x{:03x}: /* ACI data */'.format(case_hex))
    print("{ \n} break;\n")

    for sss in range(0, REGS):
        if sss == 6:
            continue
        case_bin = '10010{0:03b}'.format(sss)
        case_hex = int(case_bin, 2)
        print('case 0x{:03x}: /* SUB {} */'.format(case_hex, register[sss]))
    print("{ \n} break;\n")

    case_bin = '10010110'
    case_hex = int(case_bin, 2)
    print('case 0x{:03x}: /* SUB M */'.format(case_hex))
    print("{ \n} break;\n")

    case_bin = '11010110'
    case_hex = int(case_bin, 2)
    print('case 0x{:03x}: /* SUI data */'.format(case_hex))
    print("{ \n} break;\n")

    for sss in range(0, REGS):
        if sss == 6:
            continue
        case_bin = '10011{0:03b}'.format(sss)
        case_hex = int(case_bin, 2)
        print('case 0x{:03x}: /* SBB {} */'.format(case_hex, register[sss]))
    print("{ \n} break;\n")

    case_bin = '10011110'
    case_hex = int(case_bin, 2)
    print('case 0x{:03x}: /* SBB M */'.format(case_hex))
    print("{ \n} break;\n")

    case_bin = '11011110'
    case_hex = int(case_bin, 2)
    print('case 0x{:03x}: /* SBI data */'.format(case_hex))
    print("{ \n} break;\n")

    for ddd in range(0, REGS):
        if ddd == 6:
            continue
        case_bin = '00{0:03b}100'.format(ddd)
        case_hex = int(case_bin, 2)
        print('case 0x{:03x}: /* INR {}, data */'.format(case_hex, register[ddd]))
    print("{ \n} break;\n")

    case_bin = '00110100'
    case_hex = int(case_bin, 2)
    print('case 0x{:03x}: /* INR M */'.format(case_hex))
    print("{ \n} break;\n")

    for ddd in range(0, REGS):
        if ddd == 6:
            continue
        case_bin = '00{0:03b}101'.format(ddd)
        case_hex = int(case_bin, 2)
        print('case 0x{:03x}: /* DCR {}, data */'.format(case_hex, register[ddd]))
    print("{ \n} break;\n")

    case_bin = '00110101'
    case_hex = int(case_bin, 2)
    print('case 0x{:03x}: /* DCR M */'.format(case_hex))
    print("{ \n} break;\n")

    for rp in range(0, RPS):
        case_bin = '00{0:02b}0011'.format(rp)
        case_hex = int(case_bin, 2)
        print('case 0x{:03x}: /* INX {} */'.format(case_hex, register_pair[rp]))
    print("{ \n} break;\n")

    for rp in range(0, RPS):
        case_bin = '00{0:02b}1011'.format(rp)
        case_hex = int(case_bin, 2)
        print('case 0x{:03x}: /* DCX {} */'.format(case_hex, register_pair[rp]))
    print("{ \n} break;\n")

    for rp in range(0, RPS):
        case_bin = '00{0:02b}1001'.format(rp)
        case_hex = int(case_bin, 2)
        print('case 0x{:03x}: /* DAD {} */'.format(case_hex, register_pair[rp]))
    print("{ \n} break;\n")

    case_bin = '00100111'
    case_hex = int(case_bin, 2)
    print('case 0x{:03x}: /* DAA */'.format(case_hex))
    print("{ \n} break;\n")
    print("")

def logical_group():
    print("/* LOGICAL GROUP */")
    print("")

    for sss in range(0, REGS):
        if sss == 6:
            continue
        case_bin = '10100{0:03b}'.format(sss)
        case_hex = int(case_bin, 2)
        print('case 0x{:03x}: /* ANA {} */'.format(case_hex, register[sss]))
    print("{ \n} break;\n")

    case_bin = '10100110'
    case_hex = int(case_bin, 2)
    print('case 0x{:03x}: /* ANA M */'.format(case_hex))
    print("{ \n} break;\n")

    case_bin = '11100110'
    case_hex = int(case_bin, 2)
    print('case 0x{:03x}: /* ANI data */'.format(case_hex))
    print("{ \n} break;\n")

    for sss in range(0, REGS):
        if sss == 6:
            continue
        case_bin = '10101{0:03b}'.format(sss)
        case_hex = int(case_bin, 2)
        print('case 0x{:03x}: /* XRA {} */'.format(case_hex, register[sss]))
    print("{ \n} break;\n")

    case_bin = '10101110'
    case_hex = int(case_bin, 2)
    print('case 0x{:03x}: /* XRA M */'.format(case_hex))
    print("{ \n} break;\n")

    case_bin = '11101110'
    case_hex = int(case_bin, 2)
    print('case 0x{:03x}: /* XRI data */'.format(case_hex))
    print("{ \n} break;\n")

    for sss in range(0, REGS):
        if sss == 6:
            continue
        case_bin = '10110{0:03b}'.format(sss)
        case_hex = int(case_bin, 2)
        print('case 0x{:03x}: /* ORA {} */'.format(case_hex, register[sss]))
    print("{ \n} break;\n")

    case_bin = '10110110'
    case_hex = int(case_bin, 2)
    print('case 0x{:03x}: /* ORA M */'.format(case_hex))
    print("{ \n} break;\n")

    case_bin = '11110110'
    case_hex = int(case_bin, 2)
    print('case 0x{:03x}: /* ORI data */'.format(case_hex))
    print("{ \n} break;\n")

    for sss in range(0, REGS):
        if sss == 6:
            continue
        case_bin = '10111{0:03b}'.format(sss)
        case_hex = int(case_bin, 2)
        print('case 0x{:03x}: /* CMP {} */'.format(case_hex, register[sss]))
    print("{ \n} break;\n")

    case_bin = '10111110'
    case_hex = int(case_bin, 2)
    print('case 0x{:03x}: /* CMP M */'.format(case_hex))
    print("{ \n} break;\n")

    case_bin = '11111110'
    case_hex = int(case_bin, 2)
    print('case 0x{:03x}: /* CPI data */'.format(case_hex))
    print("{ \n} break;\n")

    case_bin = '00000111'
    case_hex = int(case_bin, 2)
    print('case 0x{:03x}: /* RLC */'.format(case_hex))
    print("{ \n} break;\n")

    case_bin = '00001111'
    case_hex = int(case_bin, 2)
    print('case 0x{:03x}: /* RRC */'.format(case_hex))
    print("{ \n} break;\n")

    case_bin = '00010111'
    case_hex = int(case_bin, 2)
    print('case 0x{:03x}: /* RAL */'.format(case_hex))
    print("{ \n} break;\n")

    case_bin = '00011111'
    case_hex = int(case_bin, 2)
    print('case 0x{:03x}: /* RAR */'.format(case_hex))
    print("{ \n} break;\n")

    case_bin = '00101111'
    case_hex = int(case_bin, 2)
    print('case 0x{:03x}: /* CMA */'.format(case_hex))
    print("{ \n} break;\n")

    case_bin = '00111111'
    case_hex = int(case_bin, 2)
    print('case 0x{:03x}: /* CMC */'.format(case_hex))
    print("{ \n} break;\n")

    case_bin = '00110111'
    case_hex = int(case_bin, 2)
    print('case 0x{:03x}: /* STC */'.format(case_hex))
    print("{ \n} break;\n")
    print("")

def branch_group():
    print("/* BRANCH GROUP */")
    print("")

    case_bin = '11000011'
    case_hex = int(case_bin, 2)
    print('case 0x{:03x}: /* JMP addr */'.format(case_hex))
    print("{ \n} break;\n")

    for ccc in range(0, CONDS):
        case_bin = '11{0:03b}010'.format(ccc)
        case_hex = int(case_bin, 2)
        print('case 0x{:03x}: /* J{: <3} addr */'.format(case_hex, condition[ccc]))
    print("{ \n} break;\n")

    case_bin = '11001101'
    case_hex = int(case_bin, 2)
    print('case 0x{:03x}: /* CALL addr */'.format(case_hex))
    print("{ \n} break;\n")

    for ccc in range(0, CONDS):
        case_bin = '11{0:03b}100'.format(ccc)
        case_hex = int(case_bin, 2)
        print('case 0x{:03x}: /* C{: <3} addr */'.format(case_hex, condition[ccc]))
    print("{ \n} break;\n")

    case_bin = '11001001'
    case_hex = int(case_bin, 2)
    print('case 0x{:03x}: /* RET */'.format(case_hex))
    print("{ \n} break;\n")

    for ccc in range(0, CONDS):
        case_bin = '11{0:03b}000'.format(ccc)
        case_hex = int(case_bin, 2)
        print('case 0x{:03x}: /* R{: <2} */'.format(case_hex, condition[ccc]))
    print("{ \n} break;\n")

    for ccc in range(0, 8):
        case_bin = '11{0:03b}111'.format(ccc)
        case_hex = int(case_bin, 2)
        print('case 0x{:03x}: /* RST {:03x} */'.format(case_hex, ccc))
    print("{ \n} break;\n")

    case_bin = '11101001'
    case_hex = int(case_bin, 2)
    print('case 0x{:03x}: /* PCHL */'.format(case_hex))
    print("{ \n} break;\n")
    print("")

def machine_control_group():
    print("/* STACK, I/O, MACHINE CONTROL GROUP */")
    print("n")

    for rp in range(0, RPS - 1):
        case_bin = '11{0:02b}0101'.format(rp)
        case_hex = int(case_bin, 2)
        print('case 0x{:03x}: /* PUSH {} */'.format(case_hex, register_pair[rp]))
    print("{ \n} break;\n")

    case_bin = '11110101'
    case_hex = int(case_bin, 2)
    print('case 0x{:03x}: /* PUSH PSW */'.format(case_hex))
    print("{ \n} break;\n")

    for rp in range(0, RPS - 1):
        case_bin = '11{0:02b}0001'.format(rp)
        case_hex = int(case_bin, 2)
        print('case 0x{:03x}: /* POP {} */'.format(case_hex, register_pair[rp]))
    print("{ \n} break;\n")

    case_bin = '11110001'
    case_hex = int(case_bin, 2)
    print('case 0x{:03x}: /* POP PSW */'.format(case_hex))
    print("{ \n} break;\n")

    case_bin = '11100011'
    case_hex = int(case_bin, 2)
    print('case 0x{:03x}: /* XTHL */'.format(case_hex))
    print("{ \n} break;\n")

    case_bin = '11111001'
    case_hex = int(case_bin, 2)
    print('case 0x{:03x}: /* SPHL */'.format(case_hex))
    print("{ \n} break;\n")

    case_bin = '11011011'
    case_hex = int(case_bin, 2)
    print('case 0x{:03x}: /* IN port */'.format(case_hex))
    print("{ \n} break;\n")

    case_bin = '11010011'
    case_hex = int(case_bin, 2)
    print('case 0x{:03x}: /* OUT port */'.format(case_hex))
    print("{ \n} break;\n")

    case_bin = '11111011'
    case_hex = int(case_bin, 2)
    print('case 0x{:03x}: /* EI */'.format(case_hex))
    print("{ \n} break;\n")

    case_bin = '11110011'
    case_hex = int(case_bin, 2)
    print('case 0x{:03x}: /* DI */'.format(case_hex))
    print("{ \n} break;\n")

    case_bin = '01110110'
    case_hex = int(case_bin, 2)
    print('case 0x{:03x}: /* HLT */'.format(case_hex))
    print("{ \n} break;\n")

    case_bin = '00000000'
    case_hex = int(case_bin, 2)
    print('case 0x{:03x}: /* NOP */'.format(case_hex))
    print("{ \n} break;\n")

def main():
    data_transfer_group()
    arithmetic_group()
    logical_group()
    branch_group()
    machine_control_group()

if __name__ == "__main__":
    main()
