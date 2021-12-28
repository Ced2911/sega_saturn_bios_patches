const { exec } = require("child_process");

function output_ld_script(sections) {
    const tpl = `
OUTPUT_FORMAT("elf32-sh")

SECTIONS
{
    ${sections.map(section=> {
        return `
    .${section.text} ${section.addr}: ALIGN_WITH_INPUT SUBALIGN(2) { KEEP(*(.${section.text})) KEEP(*(.${section.text}.*)) }
        `
    }).join('')}
    .text (NOLOAD) : { *(.text) }
    .data (NOLOAD) : { *(.data) }
    __bss_start = .;
    .bss (NOLOAD) : { *(.bss) }
    __bss_end = .;
    _end = .;
}`;
return tpl
}

exec(`sh-elf-readelf -S ${process.argv[2]}`, (error, stdout, stderr) => {
    if (error) {
        console.log(`error: ${error.message}`);
        return;
    }
    if (stderr) {
        console.log(`stderr: ${stderr}`);
        return;
    }
   // console.log(`stdout: ${stdout}`);

    const regex = /(patch([0x\da-f]+)_s)/gm;
    const sections = []
    while ((m = regex.exec(stdout)) !== null) {
        if (m.index === regex.lastIndex) {
            regex.lastIndex++;
        }
        const k=['s','text', 'addr']
        const section = {}
        m.forEach((match, groupIndex) => {
            section[k[groupIndex]]=match
        });
        sections.push(section)
    }
   // console.log(sections)
    console.log(output_ld_script(sections))
});
