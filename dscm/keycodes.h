/* Mapping key names to keycodes. */
static SCM keycodes;
static SCM keycodes_mouse;

static inline void
dscm_keycodes_initialize()
{
	scm_permanent_object(keycodes);
	scm_permanent_object(keycodes_mouse);

	keycodes = scm_make_hash_table(scm_from_int(211));
	keycodes_mouse = scm_make_hash_table(scm_from_int(3));

	/* Expose keycodes */
	scm_c_define("dwl:%keycodes", keycodes);
	scm_c_define("dwl:%keycodes-mouse", keycodes_mouse);

	scm_hash_set_x(keycodes, scm_from_locale_string("<escape>"), scm_from_int(9));
	scm_hash_set_x(keycodes, scm_from_locale_string("1"), scm_from_int(10));
	scm_hash_set_x(keycodes, scm_from_locale_string("2"), scm_from_int(11));
	scm_hash_set_x(keycodes, scm_from_locale_string("3"), scm_from_int(12));
	scm_hash_set_x(keycodes, scm_from_locale_string("4"), scm_from_int(13));
	scm_hash_set_x(keycodes, scm_from_locale_string("5"), scm_from_int(14));
	scm_hash_set_x(keycodes, scm_from_locale_string("6"), scm_from_int(15));
	scm_hash_set_x(keycodes, scm_from_locale_string("7"), scm_from_int(16));
	scm_hash_set_x(keycodes, scm_from_locale_string("8"), scm_from_int(17));
	scm_hash_set_x(keycodes, scm_from_locale_string("9"), scm_from_int(18));
	scm_hash_set_x(keycodes, scm_from_locale_string("0"), scm_from_int(19));
	scm_hash_set_x(keycodes, scm_from_locale_string("-"), scm_from_int(20));
	scm_hash_set_x(keycodes, scm_from_locale_string("="), scm_from_int(21));
	scm_hash_set_x(keycodes, scm_from_locale_string("<backspace>"), scm_from_int(22));
	scm_hash_set_x(keycodes, scm_from_locale_string("<tab>"), scm_from_int(23));
	scm_hash_set_x(keycodes, scm_from_locale_string("q"), scm_from_int(24));
	scm_hash_set_x(keycodes, scm_from_locale_string("w"), scm_from_int(25));
	scm_hash_set_x(keycodes, scm_from_locale_string("e"), scm_from_int(26));
	scm_hash_set_x(keycodes, scm_from_locale_string("r"), scm_from_int(27));
	scm_hash_set_x(keycodes, scm_from_locale_string("t"), scm_from_int(28));
	scm_hash_set_x(keycodes, scm_from_locale_string("y"), scm_from_int(29));
	scm_hash_set_x(keycodes, scm_from_locale_string("u"), scm_from_int(30));
	scm_hash_set_x(keycodes, scm_from_locale_string("i"), scm_from_int(31));
	scm_hash_set_x(keycodes, scm_from_locale_string("o"), scm_from_int(32));
	scm_hash_set_x(keycodes, scm_from_locale_string("p"), scm_from_int(33));
	scm_hash_set_x(keycodes, scm_from_locale_string("["), scm_from_int(34));
	scm_hash_set_x(keycodes, scm_from_locale_string("]"), scm_from_int(35));
	scm_hash_set_x(keycodes, scm_from_locale_string("<return>"), scm_from_int(36));
	scm_hash_set_x(keycodes, scm_from_locale_string("a"), scm_from_int(38));
	scm_hash_set_x(keycodes, scm_from_locale_string("s"), scm_from_int(39));
	scm_hash_set_x(keycodes, scm_from_locale_string("d"), scm_from_int(40));
	scm_hash_set_x(keycodes, scm_from_locale_string("f"), scm_from_int(41));
	scm_hash_set_x(keycodes, scm_from_locale_string("g"), scm_from_int(42));
	scm_hash_set_x(keycodes, scm_from_locale_string("h"), scm_from_int(43));
	scm_hash_set_x(keycodes, scm_from_locale_string("j"), scm_from_int(44));
	scm_hash_set_x(keycodes, scm_from_locale_string("k"), scm_from_int(45));
	scm_hash_set_x(keycodes, scm_from_locale_string("l"), scm_from_int(46));
	scm_hash_set_x(keycodes, scm_from_locale_string(";"), scm_from_int(47));
	scm_hash_set_x(keycodes, scm_from_locale_string("'"), scm_from_int(48));
	scm_hash_set_x(keycodes, scm_from_locale_string("`"), scm_from_int(49));
	scm_hash_set_x(keycodes, scm_from_locale_string("\\"), scm_from_int(51));
	scm_hash_set_x(keycodes, scm_from_locale_string("z"), scm_from_int(52));
	scm_hash_set_x(keycodes, scm_from_locale_string("x"), scm_from_int(53));
	scm_hash_set_x(keycodes, scm_from_locale_string("c"), scm_from_int(54));
	scm_hash_set_x(keycodes, scm_from_locale_string("v"), scm_from_int(55));
	scm_hash_set_x(keycodes, scm_from_locale_string("b"), scm_from_int(56));
	scm_hash_set_x(keycodes, scm_from_locale_string("n"), scm_from_int(57));
	scm_hash_set_x(keycodes, scm_from_locale_string("m"), scm_from_int(58));
	scm_hash_set_x(keycodes, scm_from_locale_string(","), scm_from_int(59));
	scm_hash_set_x(keycodes, scm_from_locale_string("."), scm_from_int(60));
	scm_hash_set_x(keycodes, scm_from_locale_string("/"), scm_from_int(61));
	scm_hash_set_x(keycodes, scm_from_locale_string("<kp-multiply>"), scm_from_int(63));
	scm_hash_set_x(keycodes, scm_from_locale_string("<space>"), scm_from_int(65));
	scm_hash_set_x(keycodes, scm_from_locale_string("<caps-lock>"), scm_from_int(66));
	scm_hash_set_x(keycodes, scm_from_locale_string("<F1>"), scm_from_int(67));
	scm_hash_set_x(keycodes, scm_from_locale_string("<F2>"), scm_from_int(68));
	scm_hash_set_x(keycodes, scm_from_locale_string("<F3>"), scm_from_int(69));
	scm_hash_set_x(keycodes, scm_from_locale_string("<F4>"), scm_from_int(70));
	scm_hash_set_x(keycodes, scm_from_locale_string("<F5>"), scm_from_int(71));
	scm_hash_set_x(keycodes, scm_from_locale_string("<F6>"), scm_from_int(72));
	scm_hash_set_x(keycodes, scm_from_locale_string("<F7>"), scm_from_int(73));
	scm_hash_set_x(keycodes, scm_from_locale_string("<F8>"), scm_from_int(74));
	scm_hash_set_x(keycodes, scm_from_locale_string("<F9>"), scm_from_int(75));
	scm_hash_set_x(keycodes, scm_from_locale_string("<F10>"), scm_from_int(76));
	scm_hash_set_x(keycodes, scm_from_locale_string("<num-lock>"), scm_from_int(77));
	scm_hash_set_x(keycodes, scm_from_locale_string("<scroll-lock>"), scm_from_int(78));
	scm_hash_set_x(keycodes, scm_from_locale_string("<kp-home>"), scm_from_int(79));
	scm_hash_set_x(keycodes, scm_from_locale_string("<kp-7>"), scm_from_int(79));
	scm_hash_set_x(keycodes, scm_from_locale_string("<kp-up>"), scm_from_int(80));
	scm_hash_set_x(keycodes, scm_from_locale_string("<kp-8>"), scm_from_int(80));
	scm_hash_set_x(keycodes, scm_from_locale_string("<kp-prior>"), scm_from_int(81));
	scm_hash_set_x(keycodes, scm_from_locale_string("<kp-9>"), scm_from_int(81));
	scm_hash_set_x(keycodes, scm_from_locale_string("<kp-subtract>"), scm_from_int(82));
	scm_hash_set_x(keycodes, scm_from_locale_string("<kp-left>"), scm_from_int(83));
	scm_hash_set_x(keycodes, scm_from_locale_string("<kp-4>"), scm_from_int(83));
	scm_hash_set_x(keycodes, scm_from_locale_string("<kp-begin>"), scm_from_int(84));
	scm_hash_set_x(keycodes, scm_from_locale_string("<kp-5>"), scm_from_int(84));
	scm_hash_set_x(keycodes, scm_from_locale_string("<kp-right>"), scm_from_int(85));
	scm_hash_set_x(keycodes, scm_from_locale_string("<kp-6>"), scm_from_int(85));
	scm_hash_set_x(keycodes, scm_from_locale_string("<kp-add>"), scm_from_int(86));
	scm_hash_set_x(keycodes, scm_from_locale_string("<kp-end>"), scm_from_int(87));
	scm_hash_set_x(keycodes, scm_from_locale_string("<kp-1>"), scm_from_int(87));
	scm_hash_set_x(keycodes, scm_from_locale_string("<kp-down>"), scm_from_int(88));
	scm_hash_set_x(keycodes, scm_from_locale_string("<kp-2>"), scm_from_int(88));
	scm_hash_set_x(keycodes, scm_from_locale_string("<kp-next>"), scm_from_int(89));
	scm_hash_set_x(keycodes, scm_from_locale_string("<kp-3>"), scm_from_int(89));
	scm_hash_set_x(keycodes, scm_from_locale_string("<kp-insert>"), scm_from_int(90));
	scm_hash_set_x(keycodes, scm_from_locale_string("<kp-0>"), scm_from_int(90));
	scm_hash_set_x(keycodes, scm_from_locale_string("<kp-delete>"), scm_from_int(91));
	scm_hash_set_x(keycodes, scm_from_locale_string("<kp-period>"), scm_from_int(91));
	scm_hash_set_x(keycodes, scm_from_locale_string("<F11>"), scm_from_int(95));
	scm_hash_set_x(keycodes, scm_from_locale_string("<F12>"), scm_from_int(96));
	scm_hash_set_x(keycodes, scm_from_locale_string("<kp-enter>"), scm_from_int(104));
	scm_hash_set_x(keycodes, scm_from_locale_string("<kp-divide>"), scm_from_int(106));
	scm_hash_set_x(keycodes, scm_from_locale_string("<print>"), scm_from_int(107));
	scm_hash_set_x(keycodes, scm_from_locale_string("<linefeed>"), scm_from_int(109));
	scm_hash_set_x(keycodes, scm_from_locale_string("<home>"), scm_from_int(110));
	scm_hash_set_x(keycodes, scm_from_locale_string("<up>"), scm_from_int(111));
	scm_hash_set_x(keycodes, scm_from_locale_string("<prior>"), scm_from_int(112));
	scm_hash_set_x(keycodes, scm_from_locale_string("<page-up>"), scm_from_int(112));
	scm_hash_set_x(keycodes, scm_from_locale_string("<left>"), scm_from_int(113));
	scm_hash_set_x(keycodes, scm_from_locale_string("<right>"), scm_from_int(114));
	scm_hash_set_x(keycodes, scm_from_locale_string("<end>"), scm_from_int(115));
	scm_hash_set_x(keycodes, scm_from_locale_string("<down>"), scm_from_int(116));
	scm_hash_set_x(keycodes, scm_from_locale_string("<next>"), scm_from_int(117));
	scm_hash_set_x(keycodes, scm_from_locale_string("<page-down>"), scm_from_int(117));
	scm_hash_set_x(keycodes, scm_from_locale_string("<insert>"), scm_from_int(118));
	scm_hash_set_x(keycodes, scm_from_locale_string("<delete>"), scm_from_int(119));
	scm_hash_set_x(keycodes, scm_from_locale_string("<XF86AudioMute>"), scm_from_int(121));
	scm_hash_set_x(keycodes, scm_from_locale_string("<XF86AudioLowerVolume>"), scm_from_int(122));
	scm_hash_set_x(keycodes, scm_from_locale_string("<XF86AudioRaiseVolume>"), scm_from_int(123));
	scm_hash_set_x(keycodes, scm_from_locale_string("<XF86PowerOff>"), scm_from_int(124));
	scm_hash_set_x(keycodes, scm_from_locale_string("<kp-equal>"), scm_from_int(125));
	scm_hash_set_x(keycodes, scm_from_locale_string("<plusminus>"), scm_from_int(126));
	scm_hash_set_x(keycodes, scm_from_locale_string("<pause>"), scm_from_int(127));
	scm_hash_set_x(keycodes, scm_from_locale_string("<XF86LaunchA>"), scm_from_int(128));
	scm_hash_set_x(keycodes, scm_from_locale_string("<kp-decimal>"), scm_from_int(129));
	scm_hash_set_x(keycodes, scm_from_locale_string("<menu>"), scm_from_int(135));
	scm_hash_set_x(keycodes, scm_from_locale_string("<cancel>"), scm_from_int(136));
	scm_hash_set_x(keycodes, scm_from_locale_string("<redo>"), scm_from_int(137));
	scm_hash_set_x(keycodes, scm_from_locale_string("<undo>"), scm_from_int(139));
	scm_hash_set_x(keycodes, scm_from_locale_string("<XF86Copy>"), scm_from_int(141));
	scm_hash_set_x(keycodes, scm_from_locale_string("<XF86Open>"), scm_from_int(142));
	scm_hash_set_x(keycodes, scm_from_locale_string("<XF86Paste>"), scm_from_int(143));
	scm_hash_set_x(keycodes, scm_from_locale_string("<find>"), scm_from_int(144));
	scm_hash_set_x(keycodes, scm_from_locale_string("<XF86Cut>"), scm_from_int(145));
	scm_hash_set_x(keycodes, scm_from_locale_string("<help>"), scm_from_int(146));
	scm_hash_set_x(keycodes, scm_from_locale_string("<XF86MenuKB>"), scm_from_int(147));
	scm_hash_set_x(keycodes, scm_from_locale_string("<XF86Calculator>"), scm_from_int(148));
	scm_hash_set_x(keycodes, scm_from_locale_string("<XF86Sleep>"), scm_from_int(150));
	scm_hash_set_x(keycodes, scm_from_locale_string("<XF86WakeUp>"), scm_from_int(151));
	scm_hash_set_x(keycodes, scm_from_locale_string("<XF86Explorer>"), scm_from_int(152));
	scm_hash_set_x(keycodes, scm_from_locale_string("<XF86Send>"), scm_from_int(153));
	scm_hash_set_x(keycodes, scm_from_locale_string("<XF86Xfer>"), scm_from_int(155));
	scm_hash_set_x(keycodes, scm_from_locale_string("<XF86Launch1>"), scm_from_int(156));
	scm_hash_set_x(keycodes, scm_from_locale_string("<XF86Launch2>"), scm_from_int(157));
	scm_hash_set_x(keycodes, scm_from_locale_string("<XF86WWW>"), scm_from_int(158));
	scm_hash_set_x(keycodes, scm_from_locale_string("<XF86DOS>"), scm_from_int(159));
	scm_hash_set_x(keycodes, scm_from_locale_string("<XF86ScreenSaver>"), scm_from_int(160));
	scm_hash_set_x(keycodes, scm_from_locale_string("<XF86RotateWindows>"), scm_from_int(161));
	scm_hash_set_x(keycodes, scm_from_locale_string("<XF86TaskPane>"), scm_from_int(162));
	scm_hash_set_x(keycodes, scm_from_locale_string("<XF86Mail>"), scm_from_int(163));
	scm_hash_set_x(keycodes, scm_from_locale_string("<XF86Favorites>"), scm_from_int(164));
	scm_hash_set_x(keycodes, scm_from_locale_string("<XF86MyComputer>"), scm_from_int(165));
	scm_hash_set_x(keycodes, scm_from_locale_string("<XF86Back>"), scm_from_int(166));
	scm_hash_set_x(keycodes, scm_from_locale_string("<XF86Forward>"), scm_from_int(167));
	scm_hash_set_x(keycodes, scm_from_locale_string("<XF86Eject1>"), scm_from_int(169));
	scm_hash_set_x(keycodes, scm_from_locale_string("<XF86Eject2>"), scm_from_int(170));
	scm_hash_set_x(keycodes, scm_from_locale_string("<XF86AudioNext>"), scm_from_int(171));
	scm_hash_set_x(keycodes, scm_from_locale_string("<XF86AudioPlay>"), scm_from_int(172));
	scm_hash_set_x(keycodes, scm_from_locale_string("<XF86AudioPrev>"), scm_from_int(173));
	scm_hash_set_x(keycodes, scm_from_locale_string("<XF86AudioStop>"), scm_from_int(174));
	scm_hash_set_x(keycodes, scm_from_locale_string("<XF86AudioRecord>"), scm_from_int(175));
	scm_hash_set_x(keycodes, scm_from_locale_string("<XF86AudioRewind>"), scm_from_int(176));
	scm_hash_set_x(keycodes, scm_from_locale_string("<XF86Phone>"), scm_from_int(177));
	scm_hash_set_x(keycodes, scm_from_locale_string("<XF86Tools>"), scm_from_int(179));
	scm_hash_set_x(keycodes, scm_from_locale_string("<XF86HomePage>"), scm_from_int(180));
	scm_hash_set_x(keycodes, scm_from_locale_string("<XF86Reload>"), scm_from_int(181));
	scm_hash_set_x(keycodes, scm_from_locale_string("<XF86Close>"), scm_from_int(182));
	scm_hash_set_x(keycodes, scm_from_locale_string("<XF86ScrollUp>"), scm_from_int(185));
	scm_hash_set_x(keycodes, scm_from_locale_string("<XF86ScrollDown>"), scm_from_int(186));
	scm_hash_set_x(keycodes, scm_from_locale_string("("), scm_from_int(187));
	scm_hash_set_x(keycodes, scm_from_locale_string(")"), scm_from_int(188));
	scm_hash_set_x(keycodes, scm_from_locale_string("<XF86New>"), scm_from_int(189));
	scm_hash_set_x(keycodes, scm_from_locale_string("<redo2>"), scm_from_int(190));
	scm_hash_set_x(keycodes, scm_from_locale_string("<XF86Tools2>"), scm_from_int(191));
	scm_hash_set_x(keycodes, scm_from_locale_string("<XF86Launch5>"), scm_from_int(192));
	scm_hash_set_x(keycodes, scm_from_locale_string("<XF86Launch6>"), scm_from_int(193));
	scm_hash_set_x(keycodes, scm_from_locale_string("<XF86Launch7>"), scm_from_int(194));
	scm_hash_set_x(keycodes, scm_from_locale_string("<XF86Launch8>"), scm_from_int(195));
	scm_hash_set_x(keycodes, scm_from_locale_string("<XF86Launch9>"), scm_from_int(196));
	scm_hash_set_x(keycodes, scm_from_locale_string("<XF86AudioMicMute>"), scm_from_int(198));
	scm_hash_set_x(keycodes, scm_from_locale_string("<XF86TouchpadToggle>"), scm_from_int(199));
	scm_hash_set_x(keycodes, scm_from_locale_string("<XF86TouchpadOn>"), scm_from_int(200));
	scm_hash_set_x(keycodes, scm_from_locale_string("<XF86TouchpadOff>"), scm_from_int(201));
	scm_hash_set_x(keycodes, scm_from_locale_string("<mode-switch>"), scm_from_int(203));
	scm_hash_set_x(keycodes, scm_from_locale_string("<XF86AudioPlay2>"), scm_from_int(208));
	scm_hash_set_x(keycodes, scm_from_locale_string("<XF86AudioPause>"), scm_from_int(209));
	scm_hash_set_x(keycodes, scm_from_locale_string("<XF86Launch3>"), scm_from_int(210));
	scm_hash_set_x(keycodes, scm_from_locale_string("<XF86Launch4>"), scm_from_int(211));
	scm_hash_set_x(keycodes, scm_from_locale_string("<XF86LaunchB>"), scm_from_int(212));
	scm_hash_set_x(keycodes, scm_from_locale_string("<XF86Suspend>"), scm_from_int(213));
	scm_hash_set_x(keycodes, scm_from_locale_string("<XF86Close2>"), scm_from_int(214));
	scm_hash_set_x(keycodes, scm_from_locale_string("<XF86AudioPlay3>"), scm_from_int(215));
	scm_hash_set_x(keycodes, scm_from_locale_string("<XF86AudioForward>"), scm_from_int(216));
	scm_hash_set_x(keycodes, scm_from_locale_string("<print2>"), scm_from_int(218));
	scm_hash_set_x(keycodes, scm_from_locale_string("<XF86WebCam>"), scm_from_int(220));
	scm_hash_set_x(keycodes, scm_from_locale_string("<XF86AudioPreset>"), scm_from_int(221));
	scm_hash_set_x(keycodes, scm_from_locale_string("<XF86Mail2>"), scm_from_int(223));
	scm_hash_set_x(keycodes, scm_from_locale_string("<XF86Messenger>"), scm_from_int(224));
	scm_hash_set_x(keycodes, scm_from_locale_string("<XF86Search>"), scm_from_int(225));
	scm_hash_set_x(keycodes, scm_from_locale_string("<XF86Go>"), scm_from_int(226));
	scm_hash_set_x(keycodes, scm_from_locale_string("<XF86Finance>"), scm_from_int(227));
	scm_hash_set_x(keycodes, scm_from_locale_string("<XF86Game>"), scm_from_int(228));
	scm_hash_set_x(keycodes, scm_from_locale_string("<XF86Shop>"), scm_from_int(229));
	scm_hash_set_x(keycodes, scm_from_locale_string("<cancel2>"), scm_from_int(231));
	scm_hash_set_x(keycodes, scm_from_locale_string("<XF86MonBrightnessDown>"), scm_from_int(232));
	scm_hash_set_x(keycodes, scm_from_locale_string("<XF86MonBrightnessUp>"), scm_from_int(233));
	scm_hash_set_x(keycodes, scm_from_locale_string("<XF86AudioMedia>"), scm_from_int(234));
	scm_hash_set_x(keycodes, scm_from_locale_string("<XF86Display>"), scm_from_int(235));
	scm_hash_set_x(keycodes, scm_from_locale_string("<XF86KbdLightOnOff>"), scm_from_int(236));
	scm_hash_set_x(keycodes, scm_from_locale_string("<XF86KbdBrightnessDown>"), scm_from_int(237));
	scm_hash_set_x(keycodes, scm_from_locale_string("<XF86KbdBrightnessUp>"), scm_from_int(238));
	scm_hash_set_x(keycodes, scm_from_locale_string("<XF86Send2>"), scm_from_int(239));
	scm_hash_set_x(keycodes, scm_from_locale_string("<XF86Reply>"), scm_from_int(240));
	scm_hash_set_x(keycodes, scm_from_locale_string("<XF86MailForward>"), scm_from_int(241));
	scm_hash_set_x(keycodes, scm_from_locale_string("<XF86Save>"), scm_from_int(242));
	scm_hash_set_x(keycodes, scm_from_locale_string("<XF86Documents>"), scm_from_int(243));
	scm_hash_set_x(keycodes, scm_from_locale_string("<XF86Battery>"), scm_from_int(244));
	scm_hash_set_x(keycodes, scm_from_locale_string("<XF86Bluetooth>"), scm_from_int(245));
	scm_hash_set_x(keycodes, scm_from_locale_string("<XF86WLAN>"), scm_from_int(246));
	scm_hash_set_x(keycodes, scm_from_locale_string("<XF86MonBrightnessCycle>"), scm_from_int(251));
	scm_hash_set_x(keycodes, scm_from_locale_string("<XF86WWAN>"), scm_from_int(254));
	scm_hash_set_x(keycodes, scm_from_locale_string("<XF86RFKill>"), scm_from_int(255));

	scm_hash_set_x(keycodes_mouse, scm_from_locale_string("<mouse-left>"), scm_from_int(BTN_LEFT));
	scm_hash_set_x(keycodes_mouse, scm_from_locale_string("<mouse-middle>"), scm_from_int(BTN_MIDDLE));
	scm_hash_set_x(keycodes_mouse, scm_from_locale_string("<mouse-right>"), scm_from_int(BTN_RIGHT));
}
