# Spout Effects

Spout Effects is a Windows real-time visual effects pipeline. It can accept video from webcams and Spout sources, apply visual effects, and re-output the result as a virtual webcam or a Spout source.

## Installation

Clone this repo with `git clone --recurse-submodules` and open the provided solution file with Visual Studio (tested with VS 2022). Select the Release configuration and build the solution. The executable will be located in `bin/$(Platform)/Release`.

## Usage

After running the executable, the editor window will open. On the left side of this window is the Options pane. In here, you can select your input type and output type as well as some effect-specific parameters. After selecting an input type, a dropdown list will appear with existing sources of that type (for webcam, a list of webcams; for spout, a list of spout senders). Once both an input and output have been selected, you will see a preview of the input and output video streams and the output will be available for use by other programs on your system.

## Contributing

Pull requests are welcome. For major changes, please open an issue first
to discuss what you would like to change.

## License

This project uses Unlicense. Dependencies may use other licenses.
