from multiprocessing.sharedctypes import Value
import sys
import xml.etree.ElementTree as ET
import yaml
import subprocess
import os


CONFIG_NAME = "conf.yaml"
EXPECTED_NAME = "expected.yaml"


def xml_to_dict(root):
    """
    Parses the XML metrics, where `root` is the XML tree root,
    into two dictionaries.

    First is dictionary, where key is name of the functions and value
    is another dictionary, where key is the name of the metric and value
    is its measured value.
    """
    functions = root[0]
    classes = root[1]

    funcs_d = dict()
    for f in functions:
        metrics = dict()
        # Save each metric value into a dictionary
        for metric in f:
            metrics[metric.tag] = metric.text
        # And each functions metric into another dictionary
        name = f.attrib['name']
        funcs_d[name] = metrics
    return (funcs_d, None)


if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("usage: test.py METRIC_EXE TEST_FOLDER")
        exit(1)

    # Get the path to config and expected
    test_folder = sys.argv[2]
    expected_path = os.path.join(test_folder, EXPECTED_NAME)
    config_path = os.path.join(test_folder, CONFIG_NAME)

    # Read the expected result
    with open(expected_path) as f:
        expected = yaml.safe_load(f)
    funcs_exp, classes_exp = expected['functions'], expected['classes']

    # Read the configuration
    with open(config_path) as f:
        yaml_conf = yaml.safe_load(f)
        exp_func_cnt = yaml_conf['functions']
        exp_classes_cnt = yaml_conf['classes']
        sources = yaml_conf['sources']

    # prepend the test path to sources
    tests_fullpath = [os.path.join(test_folder, "in", x) for x in sources]

    # Run the metrics program
    command = [sys.argv[1], *tests_fullpath, '--xml', 'out.xml']
    subprocess.run(command)

    # Parse the result
    tree = ET.parse('out.xml')
    root = tree.getroot()

    funcs, classes = xml_to_dict(root)

    # Compare the expected with actual result
    success = True
    # First do functions
    for fun_name, metrics in funcs_exp.items():
        # This will happen if only function with no metrics is specified
        if metrics is None:
            # TODO: Check that the function was atleast measured
            continue
        for metric, value in metrics.items():
            # Check if function was measured
            if fun_name not in funcs:
                success = False
                print(f"{fun_name} was not measured")
                continue

            # Check if metric was measured
            if metric not in funcs[fun_name]:
                success = False
                print(f"`{metric}` key was not found in measured metrics")
                continue

            # Check if the measured value is correct
            if int(funcs[fun_name][metric]) != int(value):
                success = False
                print(f"{fun_name} -> {metric}: {value}, expected: {value}, got: {funcs[fun_name][metric]}")

    #Then do classes
    pass

    exit(0 if success else 1)