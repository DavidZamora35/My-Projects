<?php
ini_set('display_errors', 1);
ini_set('display_startup_errors', 1);
error_reporting(E_ALL);

require "vendor/autoload.php";
$client = new MongoDB\Client();
$database = $client -> data;
$collection = $database -> stocks;

$result = $collection->find([]);
?>

<!Doctype HTML>
<html>
<body>

<script src="https://www.kryogenix.org/code/browser/sorttable/sorttable.js"></script>
<table class="sortable", border=2>
<thead>
<tr style="color:blue;">
<th>Symbol</th>
<th>Name</th>
<th>Price(Intraday)</th>
<th>Change</th>
<th>Volume</th>
</tr>
</thead>

<tbody>
<?php
foreach($result as $doc){
        echo "<tr>";
        foreach($doc as $key=>$value){
                if ($key != "_id") {echo "<td>{$value}</td>";}
        }
        echo "</tr>";
}
?>
</tbody>

</table>
</body>
</html>