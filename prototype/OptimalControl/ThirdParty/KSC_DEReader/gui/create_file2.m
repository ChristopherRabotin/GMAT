function varargout = create_file2(varargin)
% CREATE_FILE2 Application M-file for create_file2.fig
%    FIG = CREATE_FILE2 launch create_file2 GUI.
%    CREATE_FILE2('callback_name', ...) invoke the named callback.

% Last Modified by GUIDE v2.0 27-Jun-2001 21:01:38

if nargin == 0  % LAUNCH GUI

	fig = openfig(mfilename,'reuse');

	% Generate a structure of handles to pass to callbacks, and store it. 
	handles = guihandles(fig);
    handles.afiles=[];
    handles.hfile=[];
    handles.bfile=[];
	guidata(fig, handles);

	if nargout > 0
		varargout{1} = fig;
	end

elseif ischar(varargin{1}) % INVOKE NAMED SUBFUNCTION OR CALLBACK

	try
		[varargout{1:nargout}] = feval(varargin{:}); % FEVAL switchyard
	catch
		disp(lasterr);
	end

end


%| ABOUT CALLBACKS:
%| GUIDE automatically appends subfunction prototypes to this file, and 
%| sets objects' callback properties to call them through the FEVAL 
%| switchyard above. This comment describes that mechanism.
%|
%| Each callback subfunction declaration has the following form:
%| <SUBFUNCTION_NAME>(H, EVENTDATA, HANDLES, VARARGIN)
%|
%| The subfunction name is composed using the object's Tag and the 
%| callback type separated by '_', e.g. 'slider2_Callback',
%| 'figure1_CloseRequestFcn', 'axis1_ButtondownFcn'.
%|
%| H is the callback object's handle (obtained using GCBO).
%|
%| EVENTDATA is empty, but reserved for future use.
%|
%| HANDLES is a structure containing handles of components in GUI using
%| tags as fieldnames, e.g. handles.figure1, handles.slider2. This
%| structure is created at GUI startup using GUIHANDLES and stored in
%| the figure's application data using GUIDATA. A copy of the structure
%| is passed to each callback.  You can store additional information in
%| this structure at GUI startup, and you can change the structure
%| during callbacks.  Call guidata(h, handles) after changing your
%| copy to replace the stored original so that subsequent callbacks see
%| the updates. Type "help guihandles" and "help guidata" for more
%| information.
%|
%| VARARGIN contains any extra arguments you have passed to the
%| callback. Specify the extra arguments by editing the callback
%| property in the inspector. By default, GUIDE sets the property to:
%| <MFILENAME>('<SUBFUNCTION_NAME>', gcbo, [], guidata(gcbo))
%| Add any extra arguments after the last argument, before the final
%| closing parenthesis.



% --------------------------------------------------------------------
function varargout = pb_ascii_Callback(h, eventdata, handles, varargin)
[flist,bdir]=multifile('ASCII Ephemeris Files');
if ~isempty(flist)
    handles.afiles=sort(cellstr([char(ones(size(flist))*bdir),char(flist)]));
    set(handles.lb_ascii,'String',handles.afiles)
    guidata(gcbo,handles)
end
    
% --------------------------------------------------------------------
function varargout = pb_header_Callback(h, eventdata, handles, varargin)
[hfile,hdir]=multifile('ASCII Ephemeris Header');
if ~isempty(hfile)
    handles.hfile=cellstr([char(ones(size(hfile))*hdir),char(hfile)]);
    set(handles.et_header,'String',char(handles.hfile))
    guidata(gcbo,handles)
end


% --------------------------------------------------------------------
function varargout = lb_ascii_Callback(h, eventdata, handles, varargin)
% Stub for Callback of the uicontrol handles.et_ascii.
disp('lb_ascii Callback not implemented yet.')


% --------------------------------------------------------------------
function varargout = et_header_Callback(h, eventdata, handles, varargin)
hfile=get(handles.et_header,'String');
if exist(hfile,'file')
    handles.hfile=hfile
    guidata(gcbo,handles)
else
    errordlg([hfile,' does not exist.'],'Error: File not found')
end

% --------------------------------------------------------------------
function varargout = et_binary_Callback(h, eventdata, handles, varargin)
bfile=get(handles.et_binary,'String');
handles.bfile=bfile;
guidata(gcbo,handles)


% --------------------------------------------------------------------
function varargout = st_binary_Callback(h, eventdata, handles, varargin)
% Stub for Callback of the uicontrol handles.st_binary.
disp('st_binary Callback not implemented yet.')


% --------------------------------------------------------------------
function varargout = pb_binary_Callback(h, eventdata, handles, varargin)
a=[char(ones(size(handles.afiles))*'TEMPBFILE')];
b=num2str([11:size(handles.afiles,1)+10]');
blist=cellstr([a,b]);
asc2bin_eph(char(handles.hfile),handles.afiles,blist);
mergemall(blist,handles.bfile);
delete('TEMPBFILE*')
guidata(gcbo,handles)


% --------------------------------------------------------------------
function varargout = pb_test_Callback(h, eventdata, handles, varargin)
test_file3


% --------------------------------------------------------------------
function varargout = pb_download_Callback(h, eventdata, handles, varargin)
web ftp://ssd.jpl.nasa.gov/pub/eph/export/ascii -browser


% --------------------------------------------------------------------
function varargout = pb_done_Callback(h, eventdata, handles, varargin)
close(gcbf)
