import os
import numpy as np
import multiprocessing
import ROOT

def generate_lund_file(file_index, output_folder, num_events_per_file):
    """
    Generates a single LUND file with `num_events_per_file` events.
    """
    file_path = os.path.join(output_folder, f"proton_electron_lund_{file_index}.lund")

    # Precompute constants
    deg_to_rad = np.pi / 180
    m_electron = 0.000511
    m_proton = 0.938

    # Buffer to accumulate file content
    content = []

    for _ in range(num_events_per_file):
        num_particles = 2  
        mass_number_of_target = 1  
        atomic_number_of_target = 1  
        target_polarization = 0  
        beam_polarization = 0  
        beam_type = 11  
        beam_energy = 10.6  
        interacted_nucleon = 2212  
        process_id = 0  
        event_weight = ROOT.gRandom.Uniform(0.2, 2)  

        content.append(f"{num_particles} {mass_number_of_target} {atomic_number_of_target} {target_polarization} {beam_polarization} {beam_type} {beam_energy} {interacted_nucleon} {process_id} {event_weight}\n")

        # === Generate random parameters for the trigger electron ===
        p_electron = ROOT.gRandom.Uniform(2, 8)  
        theta_electron = ROOT.gRandom.Uniform(5, 25) * deg_to_rad  
        phi_electron = ROOT.gRandom.Uniform(-180, 180) * deg_to_rad
        px_electron = p_electron * np.sin(theta_electron) * np.cos(phi_electron)
        py_electron = p_electron * np.sin(theta_electron) * np.sin(phi_electron)
        pz_electron = p_electron * np.cos(theta_electron)
        e_electron = np.sqrt(px_electron**2 + py_electron**2 + pz_electron**2 + m_electron**2)
        vz_electron = np.random.normal(-3, 2.5)

        content.append(f"1 -1 1 11 0 0 {px_electron:.6f} {py_electron:.6f} {pz_electron:.6f} {e_electron:.6f} {m_electron} 0 0 {vz_electron:.6f}\n")

        # === Generate random parameters for the proton ===
        p_proton = ROOT.gRandom.Uniform(0.3, 5)  
        theta_proton = ROOT.gRandom.Uniform(3, 80) * deg_to_rad  
        phi_proton = ROOT.gRandom.Uniform(-180, 180) * deg_to_rad
        px_proton = p_proton * np.sin(theta_proton) * np.cos(phi_proton)
        py_proton = p_proton * np.sin(theta_proton) * np.sin(phi_proton)
        pz_proton = p_proton * np.cos(theta_proton)
        e_proton = np.sqrt(px_proton**2 + py_proton**2 + pz_proton**2 + m_proton**2)

        content.append(f"2 1 1 2212 0 0 {px_proton:.6f} {py_proton:.6f} {pz_proton:.6f} {e_proton:.6f} {m_proton} 0 0 {vz_electron:.6f}\n")

    # Write accumulated content to file
    with open(file_path, "w") as f:
        f.writelines(content)


def generate_lund_files(output_folder, num_files, num_events_per_file):
    """
    Parallelized function to generate multiple LUND files.
    """
    os.makedirs(output_folder, exist_ok=True)

    # Use multiprocessing for parallel execution
    pool = multiprocessing.Pool(processes=multiprocessing.cpu_count())
    tasks = [(i, output_folder, num_events_per_file) for i in range(1, num_files + 1)]
    
    pool.starmap(generate_lund_file, tasks)
    pool.close()
    pool.join()
    
    print(f"Generated {num_files} LUND files with {num_events_per_file} events each.")

# === Example usage ===
output_folder = "/lustre24/expphy/volatile/clas12/manavb/proton_electron_lund_multithread"
num_files = 500  
num_events_per_file = 10000  

generate_lund_files(output_folder, num_files, num_events_per_file)
