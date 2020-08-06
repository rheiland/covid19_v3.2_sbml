from pyMCDS import pyMCDS
import numpy as np
import matplotlib.pyplot as plt
 
# load data
#mcds = pyMCDS('output00003696.xml', '../output')
fname = 'output00000008.xml'
fname = 'output00000001.xml'
mcds = pyMCDS(fname, '.')
 
# Set our z plane and get our substrate values along it
z_val = 0.00
substrate = 'oxygen'
print('substrate is ',substrate)
plane_oxy = mcds.get_concentrations(substrate, z_slice=z_val)
 
# Get the 2D mesh for contour plotting
#xx, yy = mcds.get_mesh()
xx, yy, zz = mcds.get_mesh()
 
# We want to be able to control the number of contour levels so we
# need to do a little set up
#num_levels = 21
num_levels = 10
min_conc = plane_oxy.min()
max_conc = plane_oxy.max()
print("min, max conc = ",min_conc, max_conc)
my_levels = np.linspace(min_conc, max_conc, num_levels)
print("my_levels = ",my_levels)
 
# set up the figure area and add data layers
fig, ax = plt.subplots()
#cs = ax.contourf(xx, yy, plane_oxy, levels=my_levels)
# plot colored contour regions
cs = ax.contourf(xx[:,:,0], yy[:,:,0], plane_oxy, levels=my_levels)

# plot black contour lines
#ax.contour(xx[:,:,0], yy[:,:,0], plane_oxy, colors='black', levels = my_levels,linewidths=0.5)
           
# Now we need to add our color bar
cbar1 = fig.colorbar(cs, shrink=0.75)
cbar1.set_label('mmHg')
 
# Let's put the time in to make these look nice
ax.set_aspect('equal')
ax.set_xlabel('x (micron)')
ax.set_ylabel('y (micron)')
# ax.set_title('oxygen (mmHg) at t = {:.1f} {:s}, z = {:.2f} {:s}'.format(mcds.get_time(),mcds.data['metadata']['time_units'],z_val,mcds.data['metadata']['spatial_units'])
plt.show()