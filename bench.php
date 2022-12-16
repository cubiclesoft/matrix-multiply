<?php
	// Benchmarking tool for matrix multiplication implementations.
	// (C) 2022 CubicleSoft.  All Rights Reserved.

	if (!isset($_SERVER["argc"]) || !$_SERVER["argc"])
	{
		echo "This file is intended to be run from the command-line.";

		exit();
	}

	$rootpath = __DIR__;

	$maximplementations = 9;

	$baselinetime = 0.2;
	$baselinevariance = 0.05;

	if (is_file($rootpath . "\\matrixmult.exe"))  $matrixmult = $rootpath . "\\matrixmult.exe";
	else if (is_file($rootpath . "/matrixmult"))  $matrixmult = $rootpath . "/matrixmult";
	else
	{
		echo "Unable to locate matrixmult executable.\n";

		exit();
	}

	function GetRoundsAndTime($type, $implementation, $size)
	{
		global $rootpath, $matrixmult, $baselinetime, $baselinevariance;

		echo $implementation . " ";

		$rounds = 1;

		do
		{
			$cmd = escapeshellarg($matrixmult) . " bench " . $type . " " . $implementation . " " . $rounds . " " . $size;
			$ts = (double)trim(exec($cmd));

			if ($ts < 0.001)  $rounds *= 10;
			else if (abs($baselinetime - $ts) > $baselinevariance)  $rounds = ($rounds / $ts) * $baselinetime;
		} while ($ts < 0.001);

		$retries = 3;
		while ($rounds > 1 && $retries > 0 && abs($baselinetime - $ts) > $baselinevariance)
		{
			$cmd = escapeshellarg($matrixmult) . " bench " . $type . " " . $implementation . " " . $rounds . " " . $size;
			$ts = (double)trim(exec($cmd));

			if (abs($baselinetime - $ts) > $baselinevariance)  $rounds = ($rounds / $ts) * $baselinetime;

			$rounds = (int)$rounds;

			$retries--;
		}

		if ($rounds < 1)  $rounds = 1;

		echo "(" . number_format($ts, 2) . ")";

		return array("rounds" => $rounds, "ts" => $ts);
	}

	function RunBenchmark($type, $typestr, $outputfilebase)
	{
		global $rootpath, $matrixmult, $maximplementations;

		$fp = fopen($outputfilebase . "_ms_per_round.csv", "wb");
		$fp2 = fopen($outputfilebase . "_rounds_per_sec.csv", "wb");

		$row = array("Size");

		// Generate the baseline calculations for each implementation.
		$process = array();
		$basetimemap = array();
		for ($x = 0; $x < $maximplementations; $x++)
		{
			$process[] = true;

			$row[] = "Implementation " . ($x + 1);
		}

		fputcsv($fp, $row);
		fputcsv($fp2, $row);

		$size = 1;
		do
		{
			$processed = false;

			$row = array((string)$size);
			$row2 = array((string)$size);

			echo "Calculating " . $typestr . " " . $size . "x" . $size . " times...\n";

			for ($x = 0; $x < $maximplementations; $x++)
			{
				if (!isset($process[$x]))
				{
					$row[] = "";
					$row2[] = "";
				}
				else
				{
					if ($processed)  echo ", ";

					$result = GetRoundsAndTime($type, $x + 1, $size);

					$row[] = (string)($result["ts"] * 1000.0 / $result["rounds"]);
					$row2[] = (string)($result["rounds"] / $result["ts"]);

					if ($result["ts"] > 10.0)  unset($process[$x]);

					$processed = true;
				}
			}

			echo "\n";

			if ($processed)
			{
				fputcsv($fp, $row);
				fputcsv($fp2, $row2);
			}

			fflush($fp);
			fflush($fp2);

			if ($size < 25)  $size++;
			else if ($size < 100)  $size += 5;
			else if ($size < 250)  $size += 10;
			else if ($size < 1000)  $size += 25;
			else if ($size < 10000)  $size += 100;
			else  $size += 1000;

		} while ($processed);

		fclose($fp);
		fclose($fp2);
	}

	RunBenchmark("n", "Non-contiguous", "benchmark_non_contiguous");
	RunBenchmark("c", "Contiguous", "benchmark_contiguous");
?>