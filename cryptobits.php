<?php
// cryptobits
// IoT secure communication protocol
// By Abdullah Daud, chelahmy@gmail.com
// 23 August 2020

/**
 * Encrypt string to base64 string of <cypher+iv>
 * params:
 * $msg - string
 * $key - hexstring of 16 bytes
 */
function cb_encrypt($msg, $key) {
	$method = "AES-128-CBC";
	$k = hex2bin($key);
    $ivlen = openssl_cipher_iv_length($method);
    $iv = openssl_random_pseudo_bytes($ivlen);
    $cipher = openssl_encrypt($msg, $method, $k, OPENSSL_RAW_DATA, $iv);
    if ($cipher === FALSE)
    	return FALSE;
    return base64_encode($cipher . $iv);
}

/**
 * Decrypt base64 string of <cypher+iv>
 * params:
 * $msg - base64 $string containing aes-128-cbc encrypted data followed by 16 bytes iv.
 * $key - hexstring of 16 bytes
 */
function cb_decrypt($msg, $key) {
	$method = "AES-128-CBC";
	$k = hex2bin($key);
	$pl = bin2hex(base64_decode($msg));
	$pl_len = strlen($pl);
	$cipher = hex2bin(substr($pl, 0, $pl_len - 32));
	$iv = hex2bin(substr($pl, $pl_len - 32));
	$dt = openssl_decrypt($cipher, $method, $k, OPENSSL_RAW_DATA, $iv);
	if ($dt === FALSE)
		$dt = openssl_decrypt($cipher, $method, $k, (OPENSSL_ZERO_PADDING|OPENSSL_RAW_DATA), $iv);
	return $dt;
}

/**
 * Request and response handler.
 * 1. Get the json string from the request.
 * 2. Verify that the json string contains properties 'a' and 'm'.
 * 3. Pass 'a' and 'm' to the handler function.
 * 4. Replace 'm' with the return of the handler function.
 * 5. Respond with the altered/unaltered json string.  
 */
function cb_handler($func) {
	$json = file_get_contents('php://input');
	$data = json_decode($json, true);
	if (isset($data["a"]) && isset($data["m"])) {
		if (is_callable($func))
			$data["m"] = $func($data["a"], $data["m"]);
	}
	header('Content-Type: application/json');
	echo json_encode($data);
}

?>
