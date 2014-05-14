<?php
/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Author: Yahya Mohammad <mfyahya@gmail.com>                           | 
  +----------------------------------------------------------------------+
*/
$br = (php_sapi_name() == "cli")? "":"<br>";

if(!extension_loaded('itl')) {
	dl('itl.' . PHP_SHLIB_SUFFIX);
}
$module = 'itl';
$functions = get_extension_funcs($module);
echo "Functions available in the test extension:$br\n";
foreach($functions as $func) {
    echo $func."$br\n";
}
echo "$br\n";

echo "testing: itl_h_date(10, 5, 2009)\n";
echo "output:\n";
print_r(itl_h_date(10, 5, 2009));

echo "testing: itl_g_date(17, 8, 1429)\n";
echo "output:\n";
print_r(itl_g_date(17, 8, 1429));

echo "testing itl_get_prayer_times(55.28, 25.25, 4, 6, 10, 5, 2009)\n";
echo "output:\n";
print_r(itl_get_prayer_times(55.28, 25.25, 4, 6, 10, 5, 2009));

echo "testing itl_get_north_qibla(36, 21)\n";
echo "output:\n";
echo (itl_get_north_qibla(36, 21) . "\n");

echo "testing itl_get_prayer_times with custom method\n";
print_r(itl_get_prayer_times(39.8304, 21.4206, 3,
	array(
		5.1,  // fajr angle
		3.2,  // ishaa angle
		1.5,  // imsaak angle
		0,    // fajr interval
		0,    // isha interval
		0,    // imsaak interval
		1,    // round
		1,    //  mathab (1:shafi or 2:hanafi)
		48.5, // nearest Lat
		0,    // extreme
		1,    // offset enable?
		array(3, 0, 0 ,0, 0, -10) // offsets
	),
	10, 5, 2009));
echo "testing itl_get_prayer_times with custom method with nulls\n";
print_r(itl_get_prayer_times(55.28, 25.2522, 4,
	array(
		NULL, // fajr angle
		NULL, // ishaa angle
		NULL, // imsaak angle
		NULL, // fajr interval
		NULL, // isha interval
		NULL, // imsaak interval
		NULL, // round
		NULL, // mathab (1:shafi or 2:hanafi)
		NULL, // nearest Lat
		NULL, // extreme
		NULL, // offset enable?
		NULL
	),
	10, 5, 2009));
?>
