(function () {
  function t32(hljs) {
    const MNEMONICS = [
      "LDA", "STA", "LDI", "LDP", "JSR", "RET", "ADD", "SUB",
      "CMP", "PSH", "POP", "JMP", "JEQ", "JNG", "PRT", "RTR",
      "HLT", "IDP", "DDP", "AND", "ORR", "XOR", "SHL", "SHR",
      "LDL", "LDH", "SDL", "SDH", "ADI", "SBI", "CMI", "NOP"
    ];

    return {
      name: "T32",
      case_insensitive: true,
      keywords: {
        keyword: MNEMONICS.join(" "),
        built_in: ".data .ascii"
      },
      contains: [
        hljs.COMMENT(";", "$"),
        {
          className: "symbol",
          begin: /(?:^|\s)@?[A-Za-z_][A-Za-z0-9_]*:/,
          relevance: 10
        },
        {
          className: "title",
          begin: /(?:^|\s)@?[A-Za-z_][A-Za-z0-9_]*/,
          end: /(?=\s|$)/,
          relevance: 0
        },
        {
          className: "number",
          begin: /\$[0-9A-Fa-f]+\b/
        },
        {
          className: "number",
          begin: /\b\d+\b/
        },
        {
          className: "string",
          begin: /"/,
          end: /"/,
          contains: [{ begin: /\\[nrt"\\]/ }]
        }
      ]
    };
  }

  if (typeof hljs !== "undefined") {
    hljs.registerLanguage("t32", t32);
    hljs.registerAliases(["asm", "s32"], { languageName: "t32" });
  }
})();
