<?php
// cryptobits server test

require_once("cryptobits.php");

cb_handler(function ($a, $m) {
	$my_key = "2B7E151628AED2A6ABF7158809CF4F3C"; // shared key
	$pl = cb_decrypt($m, $my_key);
	$pl = strrev($pl) . "|" . $pl;
	return cb_encrypt($pl, $my_key);
});

?>
