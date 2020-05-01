# Null Finder

Program that takes in a CSV file and outputs a list of potential null-equivalent words in the file.

Intended to be used for uncleaned or semi-cleaned data sets where the different words that may indicate a null value are unknown. Inevitably there is some noise in the results, nonetheless they are rather helpful as a guide to help you clean your data!

## Compilation

Simply `make` in root directory. You will need to clone libcsv and install or compile library's source code (see Dependency).

## Usage

```
./find_null null_file csv_file rows_num
```

where `null_file` should be `resources/nulls`, `csv_file` is the uncleaned dataset, and `rows_num` is number of rows of data in the dataset.

## Examples

Running on [steam_support_info.csv](https://www.kaggle.com/nikdavis/steam-store-games#steam_support_info.csv):

```
COLUMN 1:

COLUMN 2:
<empty>,
COLUMN 3:
<empty>,
COLUMN 4:
n/a, <empty>,
```

Running on [USVideos.csv](https://www.kaggle.com/datasnaek/youtube-new#USvideos.csv):

```
COLUMN 1:

COLUMN 2:

COLUMN 3:

COLUMN 4:

COLUMN 5:

COLUMN 6:

COLUMN 7:
[none],
COLUMN 8:

COLUMN 9:

COLUMN 10:

COLUMN 11:

COLUMN 12:

COLUMN 13:

COLUMN 14:
True,
COLUMN 15:
True,
COLUMN 16:
<empty>, 
```

## Output

As above, output shows column number (starting at 1) and a list of possible null-equivalent words in that column if any, separated by commas. `<empty>` indicates a blank field.

## Dependency

See libcsv submodule for the libcsv library (license info & source code). This library is NOT MINE in any way; I simply used it in the project.
