import os

def compile_files():
    src_dir = 'src'
    build_dir = 'build'
    include_dir = 'inc'
    
    # Creating build folder if not exist
    if not os.path.exists(build_dir):
        os.makedirs(build_dir)
    
    

    object_files = ''

    # Compiling all files in src folder who NOT start with '_'
    for root, dirs, files in os.walk(src_dir):
        for file in files:
            # Compiling only files that ends with ".asm" or ".c"
            if (file.endswith('.asm') or file.endswith('.c')):
                src_file_path = os.path.join(root, file)
                
                build_file_path = os.path.join(build_dir, os.path.splitext(file)[0] + '.o')
                
                print(f'-{os.path.basename(file)}')

                # Compiling!
                if file.endswith('.asm'):
                    os.system(f'nasm -f elf32 {src_file_path} -o {build_file_path}')
                elif file.endswith('.c'):
                    os.system(f'gcc -I{include_dir} -c {src_file_path} -o {build_file_path} -m32 -nostdlib -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs -ffreestanding -O2')
                
                object_files = f'{object_files}{build_file_path} '

    #Linking
    os.system(f'ld -m elf_i386 -T link.ld -o kernel {object_files}')

    os.system(f'rm isobuild/boot/kernel.bin')
    os.system(f'mv kernel isobuild/boot/kernel.bin')
    os.system(f'rm nexos.iso')
    os.system(f'grub-mkrescue -o nexos.iso isobuild')
    #Running an emulator
    os.system(f'qemu-system-i386 -cdrom nexos.iso')

if __name__ == "__main__":
    compile_files()
    exit()
