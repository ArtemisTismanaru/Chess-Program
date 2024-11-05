

<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  
</head>
<body>

<h1>Chess Mate Solver</h1>

<p>This project is a command-line program that solves chess puzzles of the type: "White/Black to move and checkmate in ≤ n moves." It evaluates chess positions to find all possible checkmate sequences within a specified number of moves, using multi-threading for efficient exploration.</p>

<h2>Features</h2>
<ul>
  <li>Solves checkmate problems for either White or Black within a move limit.</li>
  <li>Uses threads to explore each move in parallel, up to <code>n</code> levels deep.</li>
  <li>Maintains a list of the longest sequences leading to checkmate.</li>
  <li>Outputs the optimal sequence of moves for checkmate.</li>
</ul>

<h2>Usage</h2>
<p>Run the program from the command line with the following arguments:</p>

<pre><code>./program &lt;move_limit&gt; &lt;color&gt; &lt;piece1&gt; &lt;piece2&gt; ...
</code></pre>

<ul>
  <li><code>move_limit</code> – Maximum number of moves to checkmate.</li>
  <li><code>color</code> – Side to move (<code>a</code> for White, <code>n</code> for Black).</li>
  <li><code>piece</code> – Format: <code>&lt;color&gt;&lt;type&gt;&lt;position&gt;</code>, e.g., <code>akd2</code> (White king on d2) or <code>nrh5</code> (Black rook on h5).</li>
</ul>

<h2>Example</h2>
<p>To solve a puzzle with a 2-move checkmate limit for White:</p>

<pre><code>./program 2 a akd2 nrd5 ...
</code></pre>

<h2>How It Works</h2>
<p>The program initializes a main thread that generates positions up to <code>n</code> moves deep. Each position is evaluated for potential checkmate sequences, with each thread verifying whether a checkmate is possible within the remaining moves.</p>

<p>All discovered checkmate sequences are stored in a protected list, with only the longest sequences kept. After completing all threads, the program outputs the optimal move sequences to achieve checkmate.</p>

</body>
</html>
