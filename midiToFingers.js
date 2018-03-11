inlets = 1;
outlets = 1;

function msg_int()
{
	var input = arguments[0];
	// C
	if(input % 12 == 0) {
		if(input < 43) outlet(0, 255); // 1-111-1111
		else outlet(0, 160); // 1-010-0000
 	}
	// C#
	else if(input % 12 == 1) outlet(0, 0); // 0-000-0000
	// D
	else if(input % 12 == 2) outlet(0, 254); // 1-111-1110
	// D#/Eb 
	else if(input % 12 == 3) return;
	// E
	else if(input % 12 == 4) outlet(0, 252); // 1-111-1100
	// F
	else if(input % 12 == 5) outlet(0, 251); // 1-111-1011
	// F#/Gb
	else if(input % 12 == 6) outlet(0, 246); // 1-111-0110
	// G
	else if(input % 12 == 7) outlet(0, 240); // 1-111-0000
	// G#/Ab
	else if(input % 12 == 8) outlet(0, 236); // 1-110-1100
	// A
	else if(input % 12 == 9) outlet(0, 224); // 1-110-0000
	// A#/Bb
	else if(input % 12 == 10) outlet(0, 216); // 1-101-1000
	// B
	else if(input % 12 == 11) outlet(0, 192); // 1-100-0000
}
