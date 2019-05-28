<?xml version='1.0' encoding='ISO-8859-1' standalone='yes'?>
<tagfile>
  <compound kind="page">
    <filename>index</filename>
    <title></title>
    <name>index</name>
  </compound>
  <compound kind="file">
    <name>m66_demo.c</name>
    <path>/opt/menlinux/DRIVERS/MDIS_LL/M066/EXAMPLE/M66_DEMO/COM/</path>
    <filename>m66__demo_8c</filename>
    <member kind="define">
      <type>#define</type>
      <name>INT32_OR_64</name>
      <anchor>a0</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>U_INT32_OR_64</name>
      <anchor>a1</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>INT32_OR_64</type>
      <name>MDIS_PATH</name>
      <anchor>a3</anchor>
      <arglist></arglist>
    </member>
    <member kind="function" static="yes">
      <type>int</type>
      <name>M66_Demo</name>
      <anchor>a4</anchor>
      <arglist>(char *devName)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>void</type>
      <name>ShowError</name>
      <anchor>a5</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>main</name>
      <anchor>a6</anchor>
      <arglist>(int argc, char *argv[])</arglist>
    </member>
    <member kind="variable" static="yes">
      <type>char *</type>
      <name>RCSid</name>
      <anchor>a2</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>m66_doc.c</name>
    <path>/opt/menlinux/DRIVERS/MDIS_LL/M066/DRIVER/COM/</path>
    <filename>m66__doc_8c</filename>
  </compound>
  <compound kind="file">
    <name>m66_drv.c</name>
    <path>/opt/menlinux/DRIVERS/MDIS_LL/M066/DRIVER/COM/</path>
    <filename>m66__drv_8c</filename>
    <class kind="struct">M66_HANDLE</class>
    <member kind="define">
      <type>#define</type>
      <name>INT32_OR_64</name>
      <anchor>a0</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DBG_MYLEVEL</name>
      <anchor>a1</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DBH</name>
      <anchor>a2</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>M66_CH_WIDTH</name>
      <anchor>a3</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MOD_ID_MAGIC</name>
      <anchor>a4</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MOD_ID_SIZE</name>
      <anchor>a5</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MOD_ID</name>
      <anchor>a6</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>M66_DEFAULT_BUF_SIZE</name>
      <anchor>a7</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>M66_DEFAULT_BUF_TIMEOUT</name>
      <anchor>a8</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>M66_DEFAULT_IRQ_DISABLED</name>
      <anchor>a9</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>M66_HW_ACCESS_NO</name>
      <anchor>a10</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>M66_HW_ACCESS_PERMITED</name>
      <anchor>a11</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>M66_IRQ</name>
      <anchor>a12</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>M66_NO_IRQ</name>
      <anchor>a13</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>IOREG</name>
      <anchor>a14</anchor>
      <arglist>(ch)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>INTREG</name>
      <anchor>a15</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>D_IELH</name>
      <anchor>a16</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>D_IEHL</name>
      <anchor>a17</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>D_IEMSK</name>
      <anchor>a18</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>D_EDLH</name>
      <anchor>a19</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>D_EDHL</name>
      <anchor>a20</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>D_EDMSK</name>
      <anchor>a21</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>D_OUTVAL</name>
      <anchor>a22</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>D_INVAL</name>
      <anchor>a23</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>I_IRQCH</name>
      <anchor>a24</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>M66_FLREG</name>
      <anchor>a25</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>D302_MAGICWORD_REG</name>
      <anchor>a26</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>bitset</name>
      <anchor>a27</anchor>
      <arglist>(byte, mask)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>bitclr</name>
      <anchor>a28</anchor>
      <arglist>(byte, mask)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>bitmove</name>
      <anchor>a29</anchor>
      <arglist>(byte, mask, bool)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>int32</type>
      <name>M66_MemCleanup</name>
      <anchor>a31</anchor>
      <arglist>(M66_HANDLE *m66Hdl)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>int32</type>
      <name>M66_HwBlockRead</name>
      <anchor>a32</anchor>
      <arglist>(M66_HANDLE *m66Hdl, u_int8 *buf, int calledFromIrq, int32 *edgeOccuredP)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>int32</type>
      <name>getStatBlock</name>
      <anchor>a33</anchor>
      <arglist>(M66_HANDLE *m66Hdl, int32 code, M_SETGETSTAT_BLOCK *blockStruct)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>char *</type>
      <name>M66_Ident</name>
      <anchor>a34</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>char *</type>
      <name>M66_FlexDataIdent</name>
      <anchor>a35</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>int32</type>
      <name>M66_Init</name>
      <anchor>a36</anchor>
      <arglist>(DESC_SPEC *descSpec, OSS_HANDLE *osHdl, MACCESS *ma, OSS_SEM_HANDLE *devSemHdl, OSS_IRQ_HANDLE *irqHdl, LL_HANDLE **llHdlP)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>int32</type>
      <name>M66_Exit</name>
      <anchor>a37</anchor>
      <arglist>(LL_HANDLE **llHdlP)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>int32</type>
      <name>M66_Read</name>
      <anchor>a38</anchor>
      <arglist>(LL_HANDLE *llHdl, int32 ch, int32 *value)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>int32</type>
      <name>M66_Write</name>
      <anchor>a39</anchor>
      <arglist>(LL_HANDLE *llHdl, int32 ch, int32 value)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>int32</type>
      <name>M66_SetStat</name>
      <anchor>a40</anchor>
      <arglist>(LL_HANDLE *llHdl, int32 code, int32 ch, INT32_OR_64 value32_or_64)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>int32</type>
      <name>M66_GetStat</name>
      <anchor>a41</anchor>
      <arglist>(LL_HANDLE *llHdl, int32 code, int32 ch, INT32_OR_64 *value32_or_64P)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>int32</type>
      <name>M66_BlockRead</name>
      <anchor>a42</anchor>
      <arglist>(LL_HANDLE *llHdl, int32 ch, void *buf, int32 size, int32 *nbrRdBytesP)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>int32</type>
      <name>M66_BlockWrite</name>
      <anchor>a43</anchor>
      <arglist>(LL_HANDLE *llHdl, int32 ch, void *buf, int32 size, int32 *nbrWrBytesP)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>int32</type>
      <name>M66_Irq</name>
      <anchor>a44</anchor>
      <arglist>(LL_HANDLE *llHdl)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>int32</type>
      <name>M66_Info</name>
      <anchor>a45</anchor>
      <arglist>(int32 infoType,...)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>int32</type>
      <name>m66_flexload</name>
      <anchor>a46</anchor>
      <arglist>(M66_HANDLE *m66Hdl)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>__M66_GetEntry</name>
      <anchor>a47</anchor>
      <arglist>(LL_ENTRY *drvP)</arglist>
    </member>
    <member kind="variable" static="yes">
      <type>const char</type>
      <name>IdentString</name>
      <anchor>a30</anchor>
      <arglist>[]</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>m66_drv.h</name>
    <path>/opt/menlinux/INCLUDE/COM/MEN/</path>
    <filename>m66__drv_8h</filename>
    <member kind="define">
      <type>#define</type>
      <name>M66_VARIANT</name>
      <anchor>a0</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>_M66_GLOBNAME</name>
      <anchor>a1</anchor>
      <arglist>(var, name)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>M66_GLOBNAME</name>
      <anchor>a2</anchor>
      <arglist>(var, name)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>__M66_GetEntry</name>
      <anchor>a3</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>M66_MAX_CH</name>
      <anchor>a4</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>M66_IRQ_SOURCE</name>
      <anchor>a5</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>M66_EDGE_MASK</name>
      <anchor>a6</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>M66_SIG_EDGE_OCCURRED</name>
      <anchor>a7</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>M66_SIG_CLR_EDGE_OCCURRED</name>
      <anchor>a8</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>M66_IELH</name>
      <anchor>a9</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>M66_IEHL</name>
      <anchor>a10</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>M66_OUTVAL</name>
      <anchor>a11</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>M66_OUTLH</name>
      <anchor>a12</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>M66_OUTHL</name>
      <anchor>a13</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>INT32_OR_64</name>
      <anchor>a14</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>U_INT32_OR_64</name>
      <anchor>a15</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>INT32_OR_64</type>
      <name>MDIS_PATH</name>
      <anchor>a16</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>m66_endless.c</name>
    <path>/opt/menlinux/DRIVERS/MDIS_LL/M066/TEST/M66_ENDLESS/COM/</path>
    <filename>m66__endless_8c</filename>
    <member kind="function" static="yes">
      <type>void</type>
      <name>errShow</name>
      <anchor>a1</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>main</name>
      <anchor>a2</anchor>
      <arglist>(int argc, char *argv[])</arglist>
    </member>
    <member kind="variable" static="yes">
      <type>char *</type>
      <name>RCSid</name>
      <anchor>a0</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>m66_flex.h</name>
    <path>/opt/menlinux/DRIVERS/MDIS_LL/M066/DRIVER/COM/</path>
    <filename>m66__flex_8h</filename>
    <member kind="variable" static="yes">
      <type>const char</type>
      <name>FlexIdentString</name>
      <anchor>a0</anchor>
      <arglist>[]</arglist>
    </member>
    <member kind="variable" static="yes">
      <type>const u_int8</type>
      <name>m66_flex</name>
      <anchor>a1</anchor>
      <arglist>[]</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>m66_irq.c</name>
    <path>/opt/menlinux/DRIVERS/MDIS_LL/M066/TEST/M66_IRQ/COM/</path>
    <filename>m66__irq_8c</filename>
    <member kind="define">
      <type>#define</type>
      <name>CH_NOT</name>
      <anchor>a0</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>BUF_LH</name>
      <anchor>a1</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>BUF_HL</name>
      <anchor>a2</anchor>
      <arglist></arglist>
    </member>
    <member kind="function" static="yes">
      <type>void</type>
      <name>usage</name>
      <anchor>a8</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>void __MAPILIB</type>
      <name>SigHandler</name>
      <anchor>a9</anchor>
      <arglist>(u_int32 sigCode)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>void</type>
      <name>PrintMdisError</name>
      <anchor>a10</anchor>
      <arglist>(char *info)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>void</type>
      <name>PrintUosError</name>
      <anchor>a11</anchor>
      <arglist>(char *info)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>main</name>
      <anchor>a12</anchor>
      <arglist>(int argc, char *argv[])</arglist>
    </member>
    <member kind="variable" static="yes">
      <type>char *</type>
      <name>RCSid</name>
      <anchor>a3</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" static="yes">
      <type>u_int32</type>
      <name>G_SigCountEdges</name>
      <anchor>a4</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" static="yes">
      <type>u_int32</type>
      <name>G_SigCountHighwater</name>
      <anchor>a5</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" static="yes">
      <type>u_int32</type>
      <name>G_SigCountOthers</name>
      <anchor>a6</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" static="yes">
      <type>u_int32</type>
      <name>G_BufEdge</name>
      <anchor>a7</anchor>
      <arglist>[2][M66_MAX_CH]</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>m66_main.c</name>
    <path>/opt/menlinux/DRIVERS/MDIS_LL/M066/TEST/M66_MAIN/COM/</path>
    <filename>m66__main_8c</filename>
    <member kind="function" static="yes">
      <type>int</type>
      <name>m66_openclose</name>
      <anchor>a7</anchor>
      <arglist>(char *devName)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>int</type>
      <name>m66_test</name>
      <anchor>a8</anchor>
      <arglist>(char *devName)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>main</name>
      <anchor>a9</anchor>
      <arglist>(int argc, char *argv[])</arglist>
    </member>
    <member kind="function" static="yes">
      <type>void __MAPILIB</type>
      <name>sighdl</name>
      <anchor>a10</anchor>
      <arglist>(u_int32 sigNo)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>void</type>
      <name>errShow</name>
      <anchor>a11</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>int32</type>
      <name>CheckInteger</name>
      <anchor>a12</anchor>
      <arglist>(int32 soll, int32 ist, char cmpType, int32 quit)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>int</type>
      <name>m66_mbuf</name>
      <anchor>a13</anchor>
      <arglist>(char *devName)</arglist>
    </member>
    <member kind="variable" static="yes">
      <type>char *</type>
      <name>RCSid</name>
      <anchor>a0</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" static="yes">
      <type>const int32</type>
      <name>OFF</name>
      <anchor>a1</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" static="yes">
      <type>const int32</type>
      <name>ON</name>
      <anchor>a2</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" static="yes">
      <type>const int32</type>
      <name>T_OK</name>
      <anchor>a3</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" static="yes">
      <type>const int32</type>
      <name>T_ERROR</name>
      <anchor>a4</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" static="yes">
      <type>int</type>
      <name>SigNo</name>
      <anchor>a5</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" static="yes">
      <type>int</type>
      <name>SignalCount</name>
      <anchor>a6</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>m66_perf.c</name>
    <path>/opt/menlinux/DRIVERS/MDIS_LL/M066/TEST/M66_PERF/COM/</path>
    <filename>m66__perf_8c</filename>
    <member kind="define">
      <type>#define</type>
      <name>CALLS</name>
      <anchor>a0</anchor>
      <arglist></arglist>
    </member>
    <member kind="function" static="yes">
      <type>void</type>
      <name>errShow</name>
      <anchor>a2</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>void</type>
      <name>usage</name>
      <anchor>a3</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>main</name>
      <anchor>a4</anchor>
      <arglist>(int argc, char *argv[])</arglist>
    </member>
    <member kind="variable" static="yes">
      <type>char *</type>
      <name>RCSid</name>
      <anchor>a1</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>m66_simp.c</name>
    <path>/opt/menlinux/DRIVERS/MDIS_LL/M066/EXAMPLE/M66_SIMP/COM/</path>
    <filename>m66__simp_8c</filename>
    <member kind="function" static="yes">
      <type>int</type>
      <name>M66_Simple</name>
      <anchor>a1</anchor>
      <arglist>(char *DevName)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>void</type>
      <name>errShow</name>
      <anchor>a2</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>main</name>
      <anchor>a3</anchor>
      <arglist>(int argc, char *argv[])</arglist>
    </member>
    <member kind="variable" static="yes">
      <type>char *</type>
      <name>RCSid</name>
      <anchor>a0</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>M66_HANDLE</name>
    <filename>structM66__HANDLE.html</filename>
    <member kind="variable">
      <type>int32</type>
      <name>ownMemSize</name>
      <anchor>o0</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>u_int32</type>
      <name>dbgLevel</name>
      <anchor>o1</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>DBG_HANDLE *</type>
      <name>dbgHdl</name>
      <anchor>o2</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>OSS_HANDLE *</type>
      <name>osHdl</name>
      <anchor>o3</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>MACCESS</type>
      <name>maPld</name>
      <anchor>o4</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>MDIS_IDENT_FUNCT_TBL</type>
      <name>idFuncTbl</name>
      <anchor>o5</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>MBUF_HANDLE *</type>
      <name>inbuf</name>
      <anchor>o6</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>OSS_SIG_HANDLE *</type>
      <name>sigHdl</name>
      <anchor>o7</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>OSS_IRQ_HANDLE *</type>
      <name>irqHdl</name>
      <anchor>o8</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>u_int8</type>
      <name>edgemask</name>
      <anchor>o9</anchor>
      <arglist>[M66_MAX_CH]</arglist>
    </member>
    <member kind="variable">
      <type>u_int16</type>
      <name>irqsource</name>
      <anchor>o10</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>u_int32</type>
      <name>irqCount</name>
      <anchor>o11</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>u_int32</type>
      <name>useModulId</name>
      <anchor>o12</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>u_int8</type>
      <name>irqEnabled</name>
      <anchor>o13</anchor>
      <arglist>[M66_MAX_CH]</arglist>
    </member>
    <member kind="variable">
      <type>u_int8</type>
      <name>d302Device</name>
      <anchor>o14</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="page">
    <name>m66dummy</name>
    <title>MEN logo</title>
    <filename>m66dummy</filename>
  </compound>
</tagfile>
